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

// Mode 3 isn't finished/used yet

static void noinline drawUsageBar3(byte, byte, byte);
static void noinline drawUsageBar3_m(byte, byte, byte);

static void noinline displayMode3Static()
{

}

static void noinline displayMode3(s_monitorData* monitorData)
{
/*
	setFont(9, 8, numbersSmall, 0x30);
	setColour(COL_RED_COL1, COL_GREEN_COL1, COL_BLUE_COL1);
	//drawString_num(DPLY_SYNC_DOWN_X, DPLY_SYNC_DOWN_Y, netData->sync.down, 9999);
	drawString_num(79, 2, netData->sync.up, 9999);
	drawString_num(DPLY_MISC_LOSS_X, DPLY_MISC_LOSS_Y, netData->loss, 99);
	drawString(DPLY_MISC_IP_X, DPLY_MISC_IP_Y, netData->ip, 15);
	
	drawString_num(DPLY_MISC_PING_X, DPLY_MISC_PING_Y, netData->ping, 999);
	
	setFont(18, 16, numbersLarge, 0x30);
	setColour(COL_RED_COL2, COL_GREEN_COL2, COL_BLUE_COL2);
	//drawString_num(DPLY_RATE_DOWN_X, DPLY_RATE_DOWN_Y, netData->rate.down, 999);
	//drawString_num(DPLY_RATE_UP_X, DPLY_RATE_UP_Y, netData->rate.up, 999);

	setColour(COL_RED_COL3, COL_GREEN_COL3, COL_BLUE_COL3);
	drawUsageBar3(114, 0, 0);
	drawUsageBar3_m(0, 0, 0);
	*/
	/*byte usage;
	usage = constrain(((netData->rate.down / (float)(netData->maxRate.down + 1)) * 6) + 0.5, 0, 6);
	drawUsageBar(2, 29, usage);
	usage = constrain(((netData->rate.up / (float)(netData->maxRate.up + 1)) * 6) + 0.5, 0, 6);
	drawUsageBar(2, 77, usage);*/
}

#define BUFFSIZE_3 552
#define BUFFSIZE2_3 27

static void noinline drawUsageBar3(byte x, byte y, byte usage)
{
/*	uint buff[BUFFSIZE_3];
	
	setLocation(x, y, 46, 96);

	for(byte n=0;n<8;n++)
	{
		uint offset = (BUFFSIZE_3 * n);
//		byte offset2 = (BUFFSIZE2 * n);

		// Load background
		setColour(COL_RED_COL2,COL_GREEN_COL2,COL_BLUE_COL2);
		for(uint i=BUFFSIZE_3;i--;)
			buff[i] = calcColour(pgm_read_byte(&load2Back[i + offset]));
*/
/*
		// Add usage bars
		for(byte x=usage;x--;)
		{
			setColour(colours[x].r,colours[x].g,colours[x].b);
			//for(byte yy=0;yy<3;yy++)
			for(byte yy=3;yy--;)
			{
				//for(byte xx=0;xx<9;xx++)
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
*/
		//setLocation(x, y+(11*n), 46, 12);
	/*
		LCD_RS_PORT |= (1<<LCD_RS_BIT);
		LCD_CS_PORT &= ~(1<<LCD_CS_BIT);
		for(uint i=0;i<BUFFSIZE_3;i++)
		{
			spi_transfer(buff[i] >> 8);
			spi_transfer(buff[i]);
		}
		LCD_CS_PORT |= (1<<LCD_CS_BIT);
	}
	*/
}

static void noinline drawUsageBar3_m(byte x, byte y, byte usage)
{
	/*
	uint buff[47];
	
	setLocation(x, y, 46, 96);
	
	LCD_RS_PORT |= (1<<LCD_RS_BIT);
	LCD_CS_PORT &= ~(1<<LCD_CS_BIT);

	for(byte n=0;n<96;n++)
	{
		uint offset = (46 * n);

		// Load background
		setColour(COL_RED_COL2,COL_GREEN_COL2,COL_BLUE_COL2);
		for(byte i=46;i--;)
			buff[46 - i] = calcColour(pgm_read_byte(&load2Back[i + offset]));

		for(byte i=0;i<46;i++)
		{
			spi_transfer(buff[i] >> 8);
			spi_transfer(buff[i]);
		}
	}
	
	LCD_CS_PORT |= (1<<LCD_CS_BIT);
	*/
}

