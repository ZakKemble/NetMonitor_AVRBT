/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#include <avr/io.h>
#include <stdlib.h>
#include "common.h"
#include "display.h"
#include "devices/lcd.h"
#include "resources.h"
#include "draw.h"
#include "views/mode1.h"
#include "views/mode2.h"
#include "millis/millis.h"

static bool forceRedraw;
static bool doBlank;
static f_displayStaticFunc displayStaticFunc;
static f_displayFunc displayFunc;

static inline void drawStatic(void);

void display_setMode(byte mode)
{
	static byte displayMode = DISPLAY_MODE_BLANK;
	if(mode != displayMode || mode == DISPLAY_MODE_BLANK)
	{
		displayMode = mode;
		
		switch(mode)
		{
			case 0:
				displayStaticFunc = displayMode1Static;
				displayFunc = displayMode1;
				break;
			case 1:
				displayStaticFunc = displayMode2Static;
				displayFunc = displayMode2;
				break;
			case 255:
				displayStaticFunc = NULL;
				displayFunc = NULL;
				break;
			default:
				break;
		}
		
		drawStatic();
		forceRedraw = true;
	}
}

static inline void drawStatic()
{
	lcd_blank();
	if(displayStaticFunc != NULL)
		displayStaticFunc();
}

void display_requestBlank()
{
	doBlank = true;
}

void display_update(s_monitorData* monitorData)
{
	if(doBlank)
	{
		display_setMode(DISPLAY_MODE_BLANK);
		lcd_setAddrWindow(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
		doBlank = false;
	}

	// Only redraw if something changed
	static byte lastHistory = 255;
	if(!forceRedraw && lastHistory == monitorData->netHistoryIdx)
		return;
	lastHistory = monitorData->netHistoryIdx;
	forceRedraw = false;

#if CFG_FPS
	millis_t frameStart = millis();
#endif

	if(displayFunc != NULL)
		displayFunc(monitorData);

#if CFG_FPS
	millis_t frameTime = millis() - frameStart;
	byte fps = 1000 / frameTime;
	setFont(9, 8, numbersSmall, 0x30);
	setColour(64, 64, 64);
	draw_string_num(140, 119, fps, 2);
#endif
}
