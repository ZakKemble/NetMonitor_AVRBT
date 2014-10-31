/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#include <avr/io.h>
#include "common.h"
#include "devices/buttons.h"
#include "devices/lcd.h"
#include "display.h"
#include "btNetMon.h"
#include "millis/millis.h"

#define BTN	C1

#define BRIGHTNESS_CHANGE_INTERVAL 600 // Long press time
#define MIN_PRESS_TIME 50 // Min time for button press, for de-bouncing

#define BTN_PRESSED()	(!pinRead(BTN))

static byte resetCount;

static void adjustBrightness(millis_t*, bool*);
static void changeMode(millis_t*, bool*);

void buttons_init()
{
	pinPullup(BTN, PULLUP_ENABLE);
}

void buttons_update()
{
	static millis_t buttonStartTime;
	static bool noButton; // Don't change mode if we're changing brightness

	if(BTN_PRESSED())
		adjustBrightness(&buttonStartTime, &noButton);
	else
	{
		resetCount = 0;
		if(buttonStartTime != 0) // We've been pressing the button and we've now let go
			changeMode(&buttonStartTime, &noButton);
	}	
}

static void adjustBrightness(millis_t* buttonStartTime, bool* noButton)
{
	static byte brightness = BRIGHTNESS_HIGH;
	
	millis_t startTime = *buttonStartTime;
	if(startTime == 0) // Just started pressing the button, store time
		*buttonStartTime = millis();
	else if(millis() - startTime > BRIGHTNESS_CHANGE_INTERVAL) // We've been holding the button long enough to change brightness
	{
		// Work out what brightness is next
		if(brightness == BRIGHTNESS_HIGH)
			brightness = BRIGHTNESS_LOW;
		else if(brightness == BRIGHTNESS_LOW)
			brightness = BRIGHTNESS_MID;
		else
		{
			brightness = BRIGHTNESS_HIGH;

			// Super long hold will reset the device
			resetCount++;
			if(resetCount >= 2)
				reset();
		}

		lcd_setBrightness(brightness);

		*buttonStartTime = 0;
		*noButton = true; // We're changing brightness, so don't change mode when we let go
	}
}

static void changeMode(millis_t* buttonStartTime, bool* noButton)
{
	static byte mode;

	millis_t startTime = *buttonStartTime;

	// Only change mode if button wasn't held long enough for brightness change, but long enough for de-bouncing
	if(!*noButton && millis() - startTime > MIN_PRESS_TIME)
	{
		if(++mode >= CFG_DISPLAY_MODES)
			mode = 0;
		display_setMode(mode);
	}

	*buttonStartTime = 0;
	*noButton = false;
}
