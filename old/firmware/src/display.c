/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

#include "common.h"
#include "display.h"
#include "lcd.h"
#include "spi.h"
#include "font.h"
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>

#define LCD_SELECT() LCD_RS_PORT |= (1<<LCD_RS_BIT); \
					LCD_CS_PORT &= ~(1<<LCD_CS_BIT)
#define LCD_DESELECT() (LCD_CS_PORT |= (1<<LCD_CS_BIT))

static s_font fontData;
static s_colour colour;
static byte displayMode = 255;
static bool forceRedraw = false;

static const char* stringTable[] = {
	"Kbps",
	"KB",
	"ps",
	"ms",
	"%",
	"KBps"
};

static void noinline displayMode1Static();
static void noinline displayMode2Static();
//static void noinline displayMode3Static();
static void noinline displayMode1(s_monitorData*);
static void noinline displayMode2(s_monitorData*);
//static void noinline displayMode3(s_monitorData*);
static void inline drawStatic();
static void inline setColour(byte, byte, byte);
static void inline setFont(byte, byte, const byte*, byte);
static void inline setPixel(byte, byte, byte, uint*, uint);
static void inline sendPixel(uint);
static void inline sendPixel_blank();
static void noinline drawString_num(byte, byte, uint, uint);
static void noinline drawString(byte, byte, const char*, byte);
static void inline drawChar(byte, byte, char);
static void drawGraphBar(byte, byte);
static uint inline calcColour(byte);
static void inline setLocation(byte, byte, byte, byte);

void display_setMode(byte mode)
{
	if(mode != displayMode)
	{
		displayMode = mode;
		drawStatic();
		forceRedraw = true;
	}
}

static void inline drawStatic()
{
	lcd_blank();
	if(displayMode == 0)
		displayMode1Static();
	else if(displayMode == 1)
		displayMode2Static();
//	else if(displayMode == 2)
//		displayMode3Static();
}

void display_drawDynamic(s_monitorData* monitorData)
{
	// Only redraw if something changed
	static byte lastHistory = 255;
	if(!forceRedraw && lastHistory == monitorData->netHistoryIdx)
		return;
	lastHistory = monitorData->netHistoryIdx;
	forceRedraw = false;

#if CFG_FPS
	ulong frameStart = millis();
#endif
	
	if(displayMode == 0)
		displayMode1(monitorData);
	else if(displayMode == 1)
		displayMode2(monitorData);
//	else if(displayMode == 2)
//		displayMode3(monitorData);

#if CFG_FPS
	ulong frameTime = millis() - frameStart;
	byte fps = 1000 / frameTime;
	setFont(9, 8, numbersSmall, 0x30);
	setColour(64, 64, 64);
	drawString_num(140, 119, fps, 99);
#endif
}

static void inline setColour(byte r, byte g, byte b)
{
	colour.precision.r = (r * PRECISION) / 255;
	colour.precision.g = (g * PRECISION) / 255;
	colour.precision.b = (b * PRECISION) / 255;
	colour.foreground.r = r;
	colour.foreground.g = g;
	colour.foreground.b = b;
}

static void inline setFont(byte w, byte h, const byte* font, byte offset)
{
	fontData.width	= w;
	fontData.height	= h;
	fontData.size	= w * h;
	fontData.font	= font;
	fontData.offset	= offset;
}

static void inline setPixel(byte x, byte y, byte w, uint* buffer, uint colour)
{
	uint pos = x + (w * y);
	buffer[pos] = colour;
}

static void inline sendPixel(uint pixel)
{
	spi_transfer(pixel >> 8);
	spi_transfer(pixel);
}

static void inline sendPixel_blank()
{
	spi_transfer(0);
	spi_transfer(0);
}

static void noinline drawString_num(byte x, byte y, uint num, uint max)
{
	if(num > max)
		num = max;
	
	char c[5];
	itoa(num, c, 10);

	// Could do with better logic here
	byte align = 0;
	if(max < 10)
	{
	}		
	else if(max >= 10 && max < 100)
	{
		if(num < 10)
			align++;
	}
	else if(max < 1000)
	{
		if(num < 100)
			align++;
		if(num < 10)
			align++;
	}
	else if(max < 10000)
	{
		if(num < 1000)
			align++;
		if(num < 100)
			align++;
		if(num < 10)
			align++;
	}

	drawString(x + (fontData.width * align), y, c, 0);
	
	if(align)
	{
		char clear[align + 1];
		memset(clear, ' ', align + 1);
		clear[align] = 0x00;
		drawString(x, y, clear, 0);
	}
}

static void noinline drawString(byte x, byte y, const char* string, byte len)
{
	byte count = 0;
	while(*string)
	{
		count++;
		drawChar(x, y, *string);
		if((*string != '.' && *string != ':') || fontData.font != numbersSmall)
			x += fontData.width;
		else
		{
			if(*string != '.')
				x += 5;
			else
				x += 4;
		}			
		string++;
	}

	if(len)
	{
		while(count < len)
		{
			count++;
			drawChar(x, y, ' ');
			x += fontData.width;
		}
	}	
}

static void inline drawChar(byte x, byte y, char c)
{
	bool space = false;
	const byte* font = fontData.font;
	byte width = fontData.width;

	if(font == numbersSmall)
	{
		switch(c)
		{
			case 'k':
				c = 11;
				break;
			case 'K':
				c = 11;
				break;
			case 'b':
				c = 16;
				break;
			case 'B':
				c = 10;
				break;
			case 'm':
				c = 12;
				break;
			case 's':
				c = 14;
				break;
			case 'p':
				c = 13;
				break;
			case '.':
				width = 4;
				font = fontDot;
				c = 0;
				break;
			case ':':
				width = 5;
				font = fontColon;
				c = 0;
				break;
			case '%':
				c = 15;
				break;
			case ' ':
				space = true;
				break;
			default:
				c -= fontData.offset;
				break;
		}
	}
	else
	{
		if(c == ' ')
			space = true;
		else
			c -= fontData.offset;
	}
	
//	c = workoutNewChar(c);
	
	// Set location
	setLocation(x, y, width, fontData.height);
	
	uint size = width * fontData.height;
	
	LCD_SELECT();
	for(uint i=0;i<size;i++)
	{
		if(!space)
		{
			byte intensity = pgm_read_byte(font + (c * size) + i);
			uint pixel = calcColour(intensity);
			sendPixel(pixel);
		}
		else
			sendPixel_blank();	
	}
	LCD_DESELECT();
}

static void drawGraphBar(byte maxLen, byte barSize)
{
	barSize = map(barSize,0,255,0,maxLen);

	byte b = maxLen - barSize;
	uint a = 255 * PRECISION;
	if(b > 0)
		a /= b;
	LCD_SELECT();
	for(byte l=0;l<maxLen;l++)
	{
		if(l > barSize)
		{
			byte intensity = ~((a * (l - barSize)) / PRECISION);
			uint pixel = calcColour(intensity);
			sendPixel(pixel);
			sendPixel(pixel);
		}
		else
		{
			sendPixel_blank();
			sendPixel_blank();
		}
	}
	LCD_DESELECT();
}

/*
static char inline workoutNewChar(char c)
{
	
}
*/
static uint inline calcColour(byte intensity)
{
	byte r;
	byte g;
	byte b;
	if(intensity == 0)
	{
		return 0;
//		r = colour.background.r;
//		g = colour.background.g;
//		b = colour.background.b;
	}
	else if(intensity == 255)
	{
		r = colour.foreground.r;
		g = colour.foreground.g;
		b = colour.foreground.b;
	}
	else
	{
//		r = (colour.foreground.r * (intensity/255.0)) + (colour.background.r * (1.0 - (intensity/255.0)));
//		g = (colour.foreground.g * (intensity/255.0)) + (colour.background.g * (1.0 - (intensity/255.0)));
//		b = (colour.foreground.b * (intensity/255.0)) + (colour.background.b * (1.0 - (intensity/255.0)));
		r = (intensity * colour.precision.r) / PRECISION;
		g = (intensity * colour.precision.g) / PRECISION;
		b = (intensity * colour.precision.b) / PRECISION;
	}

	uint pixel = (((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
	
	return pixel;
}

/*
void inline orientByteData(uint* buff, byte length, byte width, byte height)
{
	// Rotate byte orientation
	uint rotatedData[length];
	for(byte y=0;y<height;y++)
	{
		for (byte x=0;x<width;x++)
			rotatedData[x * height + height - y - 1] = buff[x + y * width];
	}

	// Mirror
	for(byte i=0;i<length;i++)
	{
		byte mod = i % height;
		byte x = ((height - mod - 1) + i) - mod;
		buff[i] = rotatedData[x];
	}
}
*/
static void inline setLocation(byte x, byte y, byte w, byte h)
{
	lcd_setAddrWindow(x, y, x+w-1, y+h);
}
/*
void inline sendData(uint* buff, uint length)
{
	LCD_RS_PORT |= (1<<LCD_RS_BIT);
	LCD_CS_PORT &= ~(1<<LCD_CS_BIT);
	for(uint i=0;i<length;i++)
	{
		spi_transfer(buff[i] >> 8);
		spi_transfer(buff[i]);
	}
	LCD_CS_PORT |= (1<<LCD_CS_BIT);
}
*/

#define INCLUDEDFILE
#include "displayModes/mode1.c"
#include "displayModes/mode2.c"
//#include "displayModes/mode3.c"
