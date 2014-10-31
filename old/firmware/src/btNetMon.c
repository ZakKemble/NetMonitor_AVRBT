/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

#include "common.h"
#include "btNetMon.h"
#include "spi.h"
#include "uart.h"
#include "lcd.h"
#include "display.h"
#include "bluetooth.h"
#include "usb.h"
#include "usbdrv/usbdrv.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define BRIGHTNESS_CHANGE_INTERVAL 800 // Long press time
#define MIN_PRESS_TIME 50 // Min time for button press, for de-bouncing
#define BRIGHTNESS_HIGH 255
#define BRIGHTNESS_MID 70
#define BRIGHTNESS_LOW 10

static void init();
static void checkButton();
static void adjustBrightness(ulong*, bool*);
static void changeMode(ulong*, bool*);

int main(void) 
{
	usbDeviceDisconnect();

	// Initialize
	init();

	// Make sure USB pins don't have pull-up enabled
	USBOUT &= ~(1<<USBMINUS);
	USBOUT &= ~(1<<USBPLUS);

	util_init();
	uart_init();
	spi_init();
	lcd_init();

	delay(100);
	usbDeviceConnect();
	usbInit();

	sei();

	// Setup variables
	s_monitorData monitorData;
	monitorData.netData = (s_netData){(s_netUpDown){0,0},(s_netUpDown){0,0},(s_netUpDown){0,0},0,0,"0.0.0.0"};
	monitorData.time = (s_time){"Sat 01 Jan 00", "00:00:00"};
	for(byte i=0;i<CFG_HISTORY_COUNT;i++)
	{
		monitorData.netHistory[i].down = 255;
		monitorData.netHistory[i].up = 255;
	}

	lcd_setBrightness(BRIGHTNESS_HIGH);
	lcd_setRotation(0b01101000);
	display_setMode(0);

	while(1)
	{
		usbPoll();
		checkButton();
		display_drawDynamic(&monitorData);
		bluetooth_tick(&monitorData);
//		sleep_mode();
    }
}

static void init()
{
	clock_prescale_set(CPU_DIV);
	ACSR = (1<<ACD);
	power_twi_disable();
	power_adc_disable();
	power_timer1_disable();
	set_sleep_mode(SLEEP_MODE_IDLE);

	// Set pins as inputs with pull-up, saves a few mA
	DDRB = 0x00;
	DDRC = 0x00;
	DDRD = 0x00;
	PORTB = 0xFF;
	PORTC = 0xFF;
	PORTD = 0xFF;

	// Timer 0, for LCD brightness
	TCCR0A = (1<<WGM00);
	TCCR0B = (1<<CS00)|(1<<CS01);
	
	DDRD |= (1<<DDD6);
}

static void checkButton()
{
	static ulong buttonStartTime = 0;
	static bool noButton = false;

	if(BTN_PRESSED())
		adjustBrightness(&buttonStartTime, &noButton);
	else if(buttonStartTime != 0)
		changeMode(&buttonStartTime, &noButton);
}

static void adjustBrightness(ulong* buttonStartTime, bool* noButton)
{
	static byte brightness = BRIGHTNESS_HIGH;
	
	ulong startTime = *buttonStartTime;
	if(startTime == 0)
		*buttonStartTime = millis();
	else if((uint)(millis() - startTime) > BRIGHTNESS_CHANGE_INTERVAL)
	{
		if(brightness == BRIGHTNESS_HIGH)
			brightness = BRIGHTNESS_LOW;
		else if(brightness == BRIGHTNESS_LOW)
			brightness = BRIGHTNESS_MID;
		else
			brightness = BRIGHTNESS_HIGH;
		lcd_setBrightness(brightness);
		*buttonStartTime = 0;
		*noButton = true;
	}
}

static void changeMode(ulong* buttonStartTime, bool* noButton)
{
	static byte mode = 0;
	ulong startTime = *buttonStartTime;

	if(!*noButton && (uint)(millis() - startTime) > MIN_PRESS_TIME)
	{
		mode++;
		if(mode > CFG_DISPLAY_MODES - 1)
			mode = 0;
		display_setMode(mode);
	}
	*buttonStartTime = 0;
	*noButton = false;
}