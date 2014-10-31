/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdint.h>
#include "common.h"
#include "views/mode1.h"
#include "resources.h"
#include "draw.h"
#include "devices/lcd.h"
#include "colours.h"
#include "devices/lm73.h"

#define DPLY_SYNC_DOWN_X 2
#define DPLY_SYNC_DOWN_Y 2

#define DPLY_RATE_DOWN_X 2
#define DPLY_RATE_DOWN_Y 12

#define DPLY_SYNC_UP_X 2
#define DPLY_SYNC_UP_Y 50

#define DPLY_RATE_UP_X 2
#define DPLY_RATE_UP_Y 60

#define DPLY_MISC_PING_X 2
#define DPLY_MISC_PING_Y 96

#define DPLY_MISC_LOSS_X 56
#define DPLY_MISC_LOSS_Y 96

#define DPLY_MISC_IP_X 2
#define DPLY_MISC_IP_Y 118

#define SM_BARLENGTH	68
#define SM_BARWIDTH	2
#define SM_BARSPACE	0
#define SM_SUBBARCOUNT	2
#define SM_GRAPHBARS 25

#define BUFFSIZE 228
#define BUFFSIZE2 27

#define EMAIL_ICON_OFF 0
#define EMAIL_ICON_ON1 1
#define EMAIL_ICON_ON2 2
#define EMAIL_ICON_X 135
#define EMAIL_ICON_Y 104
#define EMAIL_ICON_W 24
#define EMAIL_ICON_H 24

static void noinline drawEmail(byte);
static void noinline drawUsageBar(byte, byte, byte);
static void noinline drawGraph(s_netHistory*, byte);

// Draw things that are only updated once
void displayMode1Static(void)
{
	setFont(9, 8, numbersSmall, 0x30);
	setColour(COL_COL1);
	draw_string(DPLY_SYNC_DOWN_X + 45, DPLY_SYNC_DOWN_Y, "Kbps", 0);
	draw_string(DPLY_SYNC_UP_X + 45, DPLY_SYNC_UP_Y, "Kbps", 0);
	draw_string(DPLY_MISC_LOSS_X + 18, DPLY_MISC_LOSS_Y, "%", 0);
	draw_string(29, DPLY_MISC_PING_Y, "ms", 0);
	
	setColour(COL_COL2);
	draw_string(58, 12, "KB", 0);
	draw_string(58, 20, "ps", 0);
	draw_string(58, 60, "KB", 0);
	draw_string(58, 68, "ps", 0);
	
	// Graph border
	setColour(COL_GRAPH_BORDER);

	setLocation(91, 0, 1, 101);
	LCD_SELECT(MODE_DATA)
	{
		for(byte i=0;i<101;i++)
		{
			byte intensity = 255 * (i / 101.0);
			uint pixel = calcColour(intensity);
			sendPixel(pixel);
		}
	}

	setLocation(91, 101, 68, 1);
	LCD_SELECT(MODE_DATA)
	{
		for(byte i=0;i<68;i++)
		{
			byte intensity = 255 * ((68 - i) / 68.0);
			uint pixel = calcColour(intensity);
			sendPixel(pixel);
		}
	}
}

void displayMode1(s_monitorData* monitorData)
{
	char strBuffer[16];

	// History graph
	drawGraph(monitorData->netHistory, monitorData->netHistoryIdx);

	// Down/up sync
	setFont(9, 8, numbersSmall, 0x30);
	setColour(COL_COL1);
	draw_string_num(DPLY_SYNC_DOWN_X, DPLY_SYNC_DOWN_Y, monitorData->netData.sync.down, 4);
	draw_string_num(DPLY_SYNC_UP_X, DPLY_SYNC_UP_Y, monitorData->netData.sync.up, 4);
	draw_string_num(DPLY_MISC_LOSS_X, DPLY_MISC_LOSS_Y, monitorData->netData.loss, 2);

	// IP
	uint32_t ip = monitorData->netData.ip;
	sprintf_P(strBuffer, PSTR("%hhu.%hhu.%hhu.%hhu"), (byte)ip, (byte)(ip>>8), (byte)(ip>>16), (byte)(ip>>24));  
	draw_string(DPLY_MISC_IP_X, DPLY_MISC_IP_Y, strBuffer, 15);

	// Ping
//	if(netData->ping > 40)
//		setColour(255, 0, 0);
	draw_string_num(DPLY_MISC_PING_X, DPLY_MISC_PING_Y, monitorData->netData.ping, 3);

	// Time
	setColour(255, 128, 0);
	sprintf_P(strBuffer, PSTR("%02hhu:%02hhu:%02hhu"), monitorData->time.hour, monitorData->time.min, monitorData->time.sec);  
	draw_string(DPLY_MISC_IP_X, DPLY_MISC_IP_Y - 10, strBuffer, 8);

	// Temperature
//	sprintf_P(strBuffer, PSTR("%hhu"), lm73_getTemp());
//	draw_string(DPLY_MISC_LOSS_X + 24, DPLY_MISC_IP_Y - 10, strBuffer, 3);

	// Current down/up rates
	setFont(18, 16, numbersLarge, 0x30);
	setColour(COL_COL2);
	draw_string_num(DPLY_RATE_DOWN_X, DPLY_RATE_DOWN_Y, monitorData->netData.rate.down, 3);
	draw_string_num(DPLY_RATE_UP_X, DPLY_RATE_UP_Y, monitorData->netData.rate.up, 3);

	// Email notification
	drawEmail(monitorData->emails);

	// Usage bars
	byte usage;
	usage = limitVal(((monitorData->netData.rate.down / (float)(monitorData->netData.maxRate.down + 1)) * 6) + 0.5, 0, 6);
	drawUsageBar(2, 29, usage);
	usage = limitVal(((monitorData->netData.rate.up / (float)(monitorData->netData.maxRate.up + 1)) * 6) + 0.5, 0, 6);
	drawUsageBar(2, 77, usage);
}

// Email notification
static void noinline drawEmail(byte emails)
{
	static byte state = EMAIL_ICON_ON1;
	if(emails)
	{
		if(state != EMAIL_ICON_ON1)
		{
			setColour(EMAIL_ICON_COL_ON1);
			state = EMAIL_ICON_ON1;
		}				
		else
		{
			setColour(EMAIL_ICON_COL_ON2);
			state = EMAIL_ICON_ON2;
		}			
	}
	else
	{
		if(state == EMAIL_ICON_OFF)
			return;
		setColour(EMAIL_ICON_COL_OFF);
		state = EMAIL_ICON_OFF;
	}

	setLocation(EMAIL_ICON_X, EMAIL_ICON_Y, EMAIL_ICON_W, EMAIL_ICON_H);

	LCD_SELECT(MODE_DATA)
	{
		for(uint i=0;i<EMAIL_ICON_W*EMAIL_ICON_H;i++)
		{
			byte intensity = pgm_read_byte(&emailIcon[i]);
			uint pixel = calcColour(intensity);
			sendPixel(pixel);
		}
	}
}

// Usage bar
static void noinline drawUsageBar(byte x, byte y, byte usage)
{
	// Bar dot colours
	s_rgb colours[6];
	colours[0] = (s_rgb){DOT1};
	colours[1] = (s_rgb){DOT2};
	colours[2] = (s_rgb){DOT3};
	colours[3] = (s_rgb){DOT4};
	colours[4] = (s_rgb){DOT5};
	colours[5] = (s_rgb){DOT6};

	setLocation(x, y, 76, 12);
	
	LCD_SELECT(MODE_DATA)
	{
		// Do in 4 parts, requires a smaller buffer
		for(byte n=0;n<4;n++)
		{
			uint offset = (BUFFSIZE * n);
			byte offset2 = (BUFFSIZE2 * n);
			uint buff[BUFFSIZE];

			// Load background
			setColour(COL_USAGE_BG);
			for(uint i=BUFFSIZE;i--;)
				buff[i] = calcColour(pgm_read_byte(&loadBack[i + offset]));

			// Add usage bar dot things
			for(byte d=usage;d--;)
			{
				setColour(colours[d].r, colours[d].g, colours[d].b);
				for(byte dy=0;dy<3;dy++)
				{
					uint pos = (76 * dy) + (9 * d) + 10;
					for(byte dx=9;dx--;)
					{
						byte data = pgm_read_byte(&loadBlock[(dx + (9 * dy)) + offset2]);
						if(data == 0)
							continue;
						uint pixColour = calcColour(data);
						buff[dx + pos] = pixColour;
					}
				}
			}

			// Send buffer
			for(uint i=0;i<BUFFSIZE;i++)
				sendPixel(buff[i]);
		}
	}
}

static void noinline drawGraph(s_netHistory* netHistory, byte netHistoryLatest)
{
	netHistoryLatest--;
	if(netHistoryLatest > CFG_HISTORY_COUNT - 1) // overflow
		netHistoryLatest = CFG_HISTORY_COUNT - 1;

	lcd_setRotation(ROTATION_BARGRAPH);
	for(byte x=0;x<SM_GRAPHBARS;x++)
	{
		byte idx = (netHistoryLatest - x);
		if(idx > CFG_HISTORY_COUNT - 1)
			idx += CFG_HISTORY_COUNT;

		//setLocation(92, x * (SM_BARWIDTH * SM_SUBBARCOUNT), SM_BARLENGTH, SM_BARWIDTH * SM_SUBBARCOUNT);

		for(byte i=0;i<SM_SUBBARCOUNT;i++)
		{
			byte barSize;
			if(i == 0) // down rate
			{
				setColour(COL_BAR_DOWN);
				barSize = netHistory[idx].down;
			}
			else // up rate
			{
				setColour(COL_BAR_UP);
				barSize = netHistory[idx].up;
			}

			setLocation((x * SM_BARWIDTH) + (SM_BARWIDTH*i) + (x*SM_SUBBARCOUNT) + 28, 92, SM_BARWIDTH, SM_BARLENGTH);
			draw_graphBar(SM_BARLENGTH, barSize);
		}
	}
	lcd_setRotation(ROTATION_NORMAL);
}
