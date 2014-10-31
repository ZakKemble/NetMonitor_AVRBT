/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#include "common.h"

s_font fontData;
s_colour colour;

static byte numDigits(ulong);
static inline void drawChar(byte, byte, char);

static byte numDigits(ulong num)
{
	if(num == 0)
		return 1;
	byte digits = 0;
	while(num)
	{
		num /= 10;
		digits++;
	}
	return digits;
}

void draw_string_num(byte x, byte y, ulong num, byte digitCount)
{
	byte digits = numDigits(num);

	if(digits > digitCount)
		digits = digitCount;

	char buff[12];
	memset(buff, ' ', sizeof(buff) - 1);
	ultoa(num, buff + (digitCount - digits), 10);

	draw_string(x, y, buff, 0);
}

void draw_string(byte x, byte y, const char* string, byte len)
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

static inline void drawChar(byte x, byte y, char c)
{
	bool space = false;
	const byte* font = fontData.font;
	byte width = fontData.width;

	// We don't have the entire ASCII character set for the fonts we're using
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
			case 'M':
				c = 17;
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

	// Some crazy optimizing going on here :I
	if(!space)
	{
		LCD_SELECT(MODE_DATA)
		{
			uint pixel = calcColour(pgm_read_byte(font + (c * size)));
			
			for(uint i=1;i<size;i++)
			{
				SPDR = pixel>>8;
				__builtin_avr_delay_cycles(33);
				SPSR;
				SPDR = pixel;
				pixel = calcColour(pgm_read_byte(font + (c * size) + i));
				loop_until_bit_is_set(SPSR, SPIF);
			}
		}
	}
	else
	{
		LCD_SELECT(MODE_DATA)
		{
			for(uint i=0;i<size;i++)
				sendPixel_blank();
		}		
	}
	
}

void draw_graphBar(byte maxLen, byte barSize)
{
	barSize = remap(barSize, 0, 255, 0, maxLen);

	uint a = 255 * _BV(PRECISION);
	if(barSize > 0)
		a /= barSize;

	LCD_SELECT(MODE_DATA)
	{
		// Clear area
		for(byte l=maxLen;l>barSize;l--)
		{
			sendPixel_blank();
			sendPixel_blank();
		}

		// Draw bar
		for(byte l=0;l<barSize;l++)
		{
			byte intensity = ((a * (byte)(barSize - l)) / _BV(PRECISION));
			uint pixel = calcColour(intensity);
			sendPixel(pixel);
			sendPixel(pixel);
		}
	}
}

void draw_KBToMBWholeFrac(int32_t in, wholeFrac_s* out)
{
	float mbs = in / 1024.0;
	byte whole = mbs;
	byte frac = round((mbs - whole) * 10);

	if(frac > 9)
		frac = 9;

	if(whole > 99)
		whole = 99;

	out->whole = whole;
	out->frac = frac;
}

/*
static char inline workoutNewChar(char c)
{
	
}
*/

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
