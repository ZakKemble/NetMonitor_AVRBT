/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#include "common.h"

#define LG_BARCOUNT 39
#define LG_BARLEN 118
#define LG_BARWIDTH	2
#define LG_SUBBARCOUNT	2

static void noinline drawGraph2(s_netHistory*, byte);
static void noinline drawDot(byte, byte);

void displayMode2Static(void)
{
	setFont(9, 8, numbersSmall, 0x30);
	setColour(COL_COL1);
	draw_string(29+4+2+6, 120, "MBps", 0);
	draw_string(101+4+2+4+6, 120, "MBps", 0);
	
	drawDot(20+6, 120);
	drawDot(92+4+6, 120);
}

void displayMode2(s_monitorData* monitorData)
{
	// History graph
	drawGraph2(monitorData->netHistory, monitorData->netHistoryIdx);

	// Current down/up rates
	setFont(9, 8, numbersSmall, 0x30);
	setColour(COL_COL1);

	wholeFrac_s mb;

	// Draw values
	draw_KBToMBWholeFrac(monitorData->netData.rate.down, &mb);
	draw_string_num(2+6, 120, mb.whole, 2);
	draw_string_num(2+18+4+6, 120, mb.frac, 1);

	draw_KBToMBWholeFrac(monitorData->netData.rate.up, &mb);
	draw_string_num(74+4+6, 120, mb.whole, 2);
	draw_string_num(74+18+4+4+6, 120, mb.frac, 1);

//	draw_string_num(2, 120, monitorData->netData.rate.down, 3);
//	draw_string_num(74, 120, monitorData->netData.rate.up, 3);
}

static void noinline drawGraph2(s_netHistory* netHistory, byte netHistoryLatest)
{
	netHistoryLatest--;
	if(netHistoryLatest > CFG_HISTORY_COUNT - 1) // overflow
		netHistoryLatest = CFG_HISTORY_COUNT - 1;

	for(byte x=0;x<LG_BARCOUNT;x++)
	{
		for(byte i=0;i<LG_SUBBARCOUNT;i++)
		{
			byte barX;
			if(x > netHistoryLatest)
				barX = (x - 1) - netHistoryLatest;
			else
				barX = LG_BARCOUNT - (netHistoryLatest - x);
			setLocation((barX*(LG_BARWIDTH*LG_SUBBARCOUNT)) + (i*2), 0, LG_BARWIDTH, LG_BARLEN);

			byte barSize;
			if(i == 0) // down rate
			{
				setColour(COL_BAR_DOWN);
				barSize = netHistory[x].down;
			}
			else // up rate
			{
				setColour(COL_BAR_UP);
				barSize = netHistory[x].up;
			}
			draw_graphBar(LG_BARLEN, barSize);
		}
	}
}

static void noinline drawDot(byte x, byte y)
{
	setLocation(x, y, 4, 8);

	LCD_SELECT(MODE_DATA)
	{
		for(byte i=0;i<4*8;i++)
		{
			byte intensity = pgm_read_byte(&fontDot[i]);
			uint pixel = calcColour(intensity);
			sendPixel(pixel);
		}
	}
}
