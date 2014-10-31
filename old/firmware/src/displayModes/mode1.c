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
#define EMAIL_ICON_COL_ON1 255,0,0
#define EMAIL_ICON_COL_ON2 255,128,0
#define EMAIL_ICON_COL_OFF 0,0,0

static void noinline drawEmail(byte);
static void noinline drawUsageBar(byte, byte, byte);
static void noinline drawGraph(s_netHistory*, byte);

// Draw things that are only updated once
static void noinline displayMode1Static()
{
	setFont(9, 8, numbersSmall, 0x30);
	setColour(COL_COL1);
	drawString(DPLY_SYNC_DOWN_X + 45, DPLY_SYNC_DOWN_Y, stringTable[STR_Kbps], 0);
	drawString(DPLY_SYNC_UP_X + 45, DPLY_SYNC_UP_Y, stringTable[STR_Kbps], 0);
	drawString(DPLY_MISC_LOSS_X + 18, DPLY_MISC_LOSS_Y, stringTable[STR_PERC], 0);
	drawString(29, DPLY_MISC_PING_Y, stringTable[STR_MS], 0);
	
	setColour(COL_COL2);
	drawString(58, 12, stringTable[STR_KB], 0);
	drawString(58, 20, stringTable[STR_PS], 0);
	drawString(58, 60, stringTable[STR_KB], 0);
	drawString(58, 68, stringTable[STR_PS], 0);
	
	// Graph border
	setColour(COL_GRAPH_BORDER);

	setLocation(91, 0, 1, 101);
	LCD_SELECT();
	for(byte i=0;i<101;i++)
	{
		byte intensity = 255 * (i / 101.0);
		uint pixel = calcColour(intensity);
		sendPixel(pixel);
	}
	LCD_DESELECT();

	setLocation(91, 101, 68, 1);
	LCD_SELECT();
	for(byte i=0;i<68;i++)
	{
		byte intensity = 255 * ((68 - i) / 68.0);
		uint pixel = calcColour(intensity);
		sendPixel(pixel);
	}
	LCD_DESELECT();
}

static void noinline displayMode1(s_monitorData* monitorData)
{
	// History graph
	drawGraph(monitorData->netHistory, monitorData->netHistoryIdx);

	// Down/up sync
	setFont(9, 8, numbersSmall, 0x30);
	setColour(COL_COL1);
	drawString_num(DPLY_SYNC_DOWN_X, DPLY_SYNC_DOWN_Y, monitorData->netData.sync.down, 9999);
	drawString_num(DPLY_SYNC_UP_X, DPLY_SYNC_UP_Y, monitorData->netData.sync.up, 9999);
	drawString_num(DPLY_MISC_LOSS_X, DPLY_MISC_LOSS_Y, monitorData->netData.loss, 99);
	drawString(DPLY_MISC_IP_X, DPLY_MISC_IP_Y, monitorData->netData.ip, 15);

	// Ping
//	if(netData->ping > 40)
//		setColour(255, 0, 0);
	drawString_num(DPLY_MISC_PING_X, DPLY_MISC_PING_Y, monitorData->netData.ping, 999);

	// IP
	setColour(255, 128, 0);
	drawString(DPLY_MISC_IP_X, DPLY_MISC_IP_Y - 10, monitorData->time.time, 8);

	// Current down/up rates
	setFont(18, 16, numbersLarge, 0x30);
	setColour(COL_COL2);
	drawString_num(DPLY_RATE_DOWN_X, DPLY_RATE_DOWN_Y, monitorData->netData.rate.down, 999);
	drawString_num(DPLY_RATE_UP_X, DPLY_RATE_UP_Y, monitorData->netData.rate.up, 999);

	// Email notification
	drawEmail(monitorData->emails);

	// Usage bars
	setColour(COL_COL3);
	byte usage;
	usage = constrain(((monitorData->netData.rate.down / (float)(monitorData->netData.maxRate.down + 1)) * 6) + 0.5, 0, 6);
	drawUsageBar(2, 29, usage);
	usage = constrain(((monitorData->netData.rate.up / (float)(monitorData->netData.maxRate.up + 1)) * 6) + 0.5, 0, 6);
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

	LCD_SELECT();
	for(uint i=0;i<EMAIL_ICON_W*EMAIL_ICON_H;i++)
	{
		byte intensity = pgm_read_byte(&emailIcon[i]);
		uint pixel = calcColour(intensity);
		sendPixel(pixel);
	}
	LCD_DESELECT();
}

// Usage bar
static void noinline drawUsageBar(byte x, byte y, byte usage)
{
	// Bar dot colours
	s_rgb colours[6];
	colours[0] = (s_rgb){231,184,23};
	colours[1] = (s_rgb){231,150,23};
	colours[2] = (s_rgb){236,127,21};
	colours[3] = (s_rgb){237,94,17};
	colours[4] = (s_rgb){240,62,14};
	colours[5] = (s_rgb){255,0,0};

	setLocation(x, y, 76, 12);

	LCD_SELECT();

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
		for(byte x=usage;x--;)
		{
			setColour(colours[x].r, colours[x].g, colours[x].b);
			for(byte yy=0;yy<3;yy++)
			{
				for(byte xx=9;xx--;)
				{
					byte data = pgm_read_byte(&loadBlock[(xx + (9 * yy)) + offset2]);
					if(data == 0)
						continue;
					uint pixColour = calcColour(data);
					setPixel(xx + 8 + (9 * x), yy, 76, buff, pixColour);
				}
			}
		}

		for(uint i=0;i<BUFFSIZE;i++)
			sendPixel(buff[i]);
	}
	LCD_DESELECT();
}

static void noinline drawGraph(s_netHistory* netHistory, byte netHistoryLatest)
{
	netHistoryLatest--;
	if(netHistoryLatest > CFG_HISTORY_COUNT - 1)
		netHistoryLatest = CFG_HISTORY_COUNT - 1;

	lcd_setRotation(0b00001000);
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
			drawGraphBar(SM_BARLENGTH, barSize);
		}
	}
	lcd_setRotation(0b01101000);
}
