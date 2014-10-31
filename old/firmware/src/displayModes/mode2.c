/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

#ifndef INCLUDEDFILE
	#error "This file should not be compiled! It is included by another file."
#endif

#define LG_BARCOUNT 39
#define LG_BARLEN 118
#define LG_BARWIDTH	2
#define LG_SUBBARCOUNT	2

static void noinline drawGraph2(s_netHistory*, byte);

static void noinline displayMode2Static()
{
	setFont(9, 8, numbersSmall, 0x30);
	setColour(COL_COL1);
	drawString(29, 120, stringTable[STR_KBps], 0);
	drawString(101, 120, stringTable[STR_KBps], 0);
}

static void noinline displayMode2(s_monitorData* monitorData)
{
	// History graph
	drawGraph2(monitorData->netHistory, monitorData->netHistoryIdx);

	// Current down/up rates
	setFont(9, 8, numbersSmall, 0x30);
	setColour(COL_COL1);
	drawString_num(2, 120, monitorData->netData.rate.down, 999);
	drawString_num(74, 120, monitorData->netData.rate.up, 999);
}

static void noinline drawGraph2(s_netHistory* netHistory, byte netHistoryLatest)
{
	netHistoryLatest--;
	if(netHistoryLatest > CFG_HISTORY_COUNT - 1)
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
			drawGraphBar(LG_BARLEN, barSize);
		}
	}
}
