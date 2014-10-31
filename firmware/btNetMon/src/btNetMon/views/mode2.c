/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#include "common.h"
#include "views/mode2.h"
#include "resources.h"
#include "draw.h"
#include "devices/lcd.h"
#include "colours.h"

#define LG_BARCOUNT 39
#define LG_BARLEN 118
#define LG_BARWIDTH	2
#define LG_SUBBARCOUNT	2

static void noinline drawGraph2(s_netHistory*, byte);

void displayMode2Static(void)
{
	setFont(9, 8, numbersSmall, 0x30);
	setColour(COL_COL1);
	draw_string(29, 120, "KBps", 0);
	draw_string(101, 120, "KBps", 0);
}

void displayMode2(s_monitorData* monitorData)
{
	// History graph
	drawGraph2(monitorData->netHistory, monitorData->netHistoryIdx);

	// Current down/up rates
	setFont(9, 8, numbersSmall, 0x30);
	setColour(COL_COL1);
	draw_string_num(2, 120, monitorData->netData.rate.down, 3);
	draw_string_num(74, 120, monitorData->netData.rate.up, 3);
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
