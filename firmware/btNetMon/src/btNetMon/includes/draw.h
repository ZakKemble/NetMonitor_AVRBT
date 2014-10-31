/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#ifndef DRAW_H_
#define DRAW_H_

#include "devices/lcd.h"
#include "drivers/spi.h"

// Floats are slow so instead we use large integers
#define PRECISION	6

extern s_font fontData;
extern s_colour colour;

void draw_string_num(byte, byte, uint, byte);
void draw_string(byte, byte, const char*, byte);
void draw_graphBar(byte, byte);

inline void setColour(byte r, byte g, byte b)
{
	colour.precision.r = (r * (_BV(PRECISION))) / 255;
	colour.precision.g = (g * (_BV(PRECISION))) / 255;
	colour.precision.b = (b * (_BV(PRECISION))) / 255;
	colour.foreground.r = r >> 3;
	colour.foreground.g = g >> 2;
	colour.foreground.b = b >> 3;
}

inline void setFont(byte w, byte h, const byte* font, byte offset)
{
	fontData.width	= w;
	fontData.height	= h;
	fontData.size	= w * h;
	fontData.font	= font;
	fontData.offset	= offset;
}
/*
inline void setPixel(byte x, byte y, byte w, uint* buffer, uint colour)
{
	uint pos = x + (w * y);
	buffer[pos] = colour;
}
*/
inline void sendPixel(uint pixel)
{
	spi_transfer_nr(pixel >> 8);
	spi_transfer_nr(pixel);
}

inline void sendPixel_blank(void)
{
	spi_transfer_nr(0);
	spi_transfer_nr(0);
}

inline uint calcColour(byte intensity)
{
	s_rgb rgb;
	s_pixel pixel;

	switch(intensity)
	{
		case 0:
//			pixel.r = colour.background.r;
//			pixel.g = colour.background.g;
//			pixel.b = colour.background.b;
			return 0;
		case 255:
			rgb.r = colour.foreground.r;
			rgb.g = colour.foreground.g;
			rgb.b = colour.foreground.b;
			break;
		default:
//			r = (colour.foreground.r * (intensity/255.0)) + (colour.background.r * (1.0 - (intensity/255.0)));
//			g = (colour.foreground.g * (intensity/255.0)) + (colour.background.g * (1.0 - (intensity/255.0)));
//			b = (colour.foreground.b * (intensity/255.0)) + (colour.background.b * (1.0 - (intensity/255.0)));
			rgb.r = ((intensity * colour.precision.r) >> PRECISION) >> 3;
			rgb.g = ((intensity * colour.precision.g) >> PRECISION) >> 2;
			rgb.b = ((intensity * colour.precision.b) >> PRECISION) >> 3;
			break;
	}

	pixel.bytes[0] = (rgb.b);
	pixel.bytes[1] = (rgb.r << 3);
	pixel.word |= (rgb.g << 5);

	//pixel.word = (((rgb.r >> 3) << 11) | ((rgb.g >> 2) << 5) | (rgb.b >> 3));
	//pixel.word = ((rgb.r << 3) | (rgb.g << 5) | rgb.b);

	return pixel.word;
}

inline void setLocation(byte x, byte y, byte w, byte h)
{
	lcd_setAddrWindow(x, y, x+w-1, y+h);
}

#endif /* DRAW_H_ */