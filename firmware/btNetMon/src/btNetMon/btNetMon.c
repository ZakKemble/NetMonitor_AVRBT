/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <string.h>
#include "common.h"
#include "btNetMon.h"
#include "millis/millis.h"
#include "drivers/spi.h"
#include "drivers/uart.h"
#include "drivers/i2c.h"
#include "devices/lcd.h"
#include "display.h"
#include "devices/bluetooth.h"
#include "devices/buttons.h"
#include "devices/lm73.h"
#include "usb.h"
#include "usbdrv/usbdrv.h"

static void init(void);

int main(void)
{
	usbDeviceDisconnect();

	// Initialize
	init();

	delay(250);
	usbDeviceConnect();
	usbInit();

	sei();

	// Setup variables
	s_monitorData monitorData;
	memset(&monitorData, 0, sizeof(s_monitorData));

	lcd_setBrightness(BRIGHTNESS_HIGH);
	lcd_setRotation(ROTATION_NORMAL);
	display_setMode(DISPLAY_MODE1);

	while(1)
	{
		usbPoll();
		buttons_update();
		bluetooth_update(&monitorData);			
		display_update(&monitorData);

//		sleep_mode();
    }
}

static void init()
{
	clock_prescale_set(CPU_DIV);

	// Disable unused things
	ACSR = _BV(ACD);
	power_adc_disable();
	power_timer1_disable();

	set_sleep_mode(SLEEP_MODE_IDLE);

	// Enable pullups on unused pins
	pinPullup(C3, PULLUP_ENABLE);
	pinPullup(B0, PULLUP_ENABLE);

	millis_init();
	uart_init();
	spi_init();
	i2c_init();
	bluetooth_init();
	lcd_init();
//	lm73_init();
	buttons_init();
}

void reset()
{
	cli();
	usbDeviceDisconnect();
	wdt_enable(WDTO_15MS);
	while(1){}
}