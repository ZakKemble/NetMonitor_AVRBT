/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

/*
 * Copyright (C) <year> Rossum
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "common.h"
#include "lcd.h"
#include "spi.h"
#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

//static byte madctl = 0;

void lcd_init()
{
	LCD_RST_DDR |= (1<<LCD_RST_BIT);
	LCD_RS_DDR |= (1<<LCD_RS_BIT);
	LCD_CS_DDR |= (1<<LCD_CS_BIT);
	
	LCD_RST_PORT |= (1<<LCD_RST_BIT);
	LCD_RS_PORT &= ~(1<<LCD_RS_BIT);
	LCD_CS_PORT |= (1<<LCD_CS_BIT);
	
	delay(120);
	LCD_RST_PORT &= ~(1<<LCD_RST_BIT);
	delay(10);
	LCD_RST_PORT |= (1<<LCD_RST_BIT);

// Init code by Rossum, MIT licence
	delay(120);
	lcd_writecommand(ST7735_SWRESET); // software reset
	delay(150);

	lcd_writecommand(ST7735_SLPOUT);  // out of sleep mode
	delay(100);

	lcd_writecommand(ST7735_FRMCTR1);  // frame rate control - normal mode
	lcd_writedata(0x01);  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
	lcd_writedata(0x2C);
	lcd_writedata(0x2D);

	lcd_writecommand(ST7735_FRMCTR2);  // frame rate control - idle mode
	lcd_writedata(0x01);  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
	lcd_writedata(0x2C);
	lcd_writedata(0x2D);

	lcd_writecommand(ST7735_FRMCTR3);  // frame rate control - partial mode
	lcd_writedata(0x01); // dot inversion mode
	lcd_writedata(0x2C);
	lcd_writedata(0x2D);
	lcd_writedata(0x01); // line inversion mode
	lcd_writedata(0x2C);
	lcd_writedata(0x2D);
  
	lcd_writecommand(ST7735_INVCTR);  // display inversion control
	lcd_writedata(0x07);  // no inversion

	lcd_writecommand(ST7735_PWCTR1);  // power control
	lcd_writedata(0xA2);
	lcd_writedata(0x02);      // -4.6V
	lcd_writedata(0x84);      // AUTO mode

	lcd_writecommand(ST7735_PWCTR2);  // power control
	lcd_writedata(0xC5);      // VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD

	lcd_writecommand(ST7735_PWCTR3);  // power control
	lcd_writedata(0x0A);      // Opamp current small
	lcd_writedata(0x00);      // Boost frequency

	lcd_writecommand(ST7735_PWCTR4);  // power control
	lcd_writedata(0x8A);      // BCLK/2, Opamp current small & Medium low
	lcd_writedata(0x2A);

	lcd_writecommand(ST7735_PWCTR5);  // power control
	lcd_writedata(0x8A);
	lcd_writedata(0xEE);

	lcd_writecommand(ST7735_VMCTR1);  // power control
	lcd_writedata(0x0E);

	lcd_writecommand(ST7735_INVOFF);    // don't invert display

	lcd_writecommand(ST7735_MADCTL);  // memory access control (directions)
	lcd_writedata(0xC8);  // row address/col address, bottom to top refresh
//	madctl = 0xC8;

	lcd_writecommand(ST7735_COLMOD);  // set color mode
	lcd_writedata(LCD_COL_MODE);        // 16-bit color

	lcd_writecommand(ST7735_CASET);  // column addr set
	lcd_writedata(0x00);
	lcd_writedata(0x00);   // XSTART = 0
	lcd_writedata(0x00);
	lcd_writedata(0x7F);   // XEND = 127

	lcd_writecommand(ST7735_RASET);  // row addr set
	lcd_writedata(0x00);
	lcd_writedata(0x00);    // XSTART = 0
	lcd_writedata(0x00);
	lcd_writedata(0x9F);    // XEND = 159

	lcd_writecommand(ST7735_GMCTRP1);
	lcd_writedata(0x0f);
	lcd_writedata(0x1a);
	lcd_writedata(0x0f);
	lcd_writedata(0x18);
	lcd_writedata(0x2f);
	lcd_writedata(0x28);
	lcd_writedata(0x20);
	lcd_writedata(0x22);
	lcd_writedata(0x1f);
	lcd_writedata(0x1b);
	lcd_writedata(0x23);
	lcd_writedata(0x37);
	lcd_writedata(0x00);
	lcd_writedata(0x07);
	lcd_writedata(0x02);
	lcd_writedata(0x10);
	lcd_writecommand(ST7735_GMCTRN1);
	lcd_writedata(0x0f);
	lcd_writedata(0x1b);
	lcd_writedata(0x0f);
	lcd_writedata(0x17);
	lcd_writedata(0x33);
	lcd_writedata(0x2c);
	lcd_writedata(0x29);
	lcd_writedata(0x2e);
	lcd_writedata(0x30);
	lcd_writedata(0x30);
	lcd_writedata(0x39);
	lcd_writedata(0x3f);
	lcd_writedata(0x00);
	lcd_writedata(0x07);
	lcd_writedata(0x03);
	lcd_writedata(0x10);
  
	lcd_writecommand(ST7735_DISPON);
	delay(100);

	lcd_writecommand(ST7735_NORON);  // normal display on
	delay(10);
}

void lcd_writecommand(byte data)
{
	LCD_RS_PORT &= ~(1<<LCD_RS_BIT);
	LCD_CS_PORT &= ~(1<<LCD_CS_BIT);
	spi_transfer(data);
	LCD_CS_PORT |= (1<<LCD_CS_BIT);
}

void lcd_writedata(byte data)
{
	LCD_RS_PORT |= (1<<LCD_RS_BIT);
	LCD_CS_PORT &= ~(1<<LCD_CS_BIT);
	spi_transfer(data);
	LCD_CS_PORT |= (1<<LCD_CS_BIT);
}

void lcd_setRotation(byte data)
{
//	madctl = data;
	lcd_writecommand(ST7735_MADCTL);  // memory access control (directions)
	lcd_writedata(data);  // row address/col address, bottom to top refresh
}

void lcd_setAddrWindow(byte x0, byte y0, byte x1, byte y1)
{
	lcd_writecommand(ST7735_CASET);  // column addr set
	lcd_writedata(0x00);
	lcd_writedata(x0 + 0);   // XSTART
	lcd_writedata(0x00);
	lcd_writedata(x1 + 0);   // XEND

	lcd_writecommand(ST7735_RASET);  // row addr set
	lcd_writedata(0x00);
	lcd_writedata(y0 + 0);    // YSTART
	lcd_writedata(0x00);
	lcd_writedata(y1 + 0);    // YEND

	lcd_writecommand(ST7735_RAMWR);  // write to RAM
}

void lcd_blank()
{
	lcd_setAddrWindow(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
	LCD_RS_PORT |= (1<<LCD_RS_BIT);
	LCD_CS_PORT &= ~(1<<LCD_CS_BIT);
	for(byte y=0;y<LCD_HEIGHT;y++)
	{
		for(byte x=0;x<LCD_WIDTH;x++)
		{
			spi_transfer(0);
			spi_transfer(0);
#if CFG_LCD_COLOUR_MODE == 3
			spi_transfer(0);
#endif
		}
	}
	LCD_CS_PORT |= (1<<LCD_CS_BIT);
}

void lcd_setBrightness(byte brightness)
{
	brightness = ~brightness;
	if(brightness == 0 || brightness == 255)
	{
		TCCR0A &= ~(1<<COM0A1);
		if(brightness)
			PORTD |= (1<<PORTD6);
		else
			PORTD &= ~(1<<PORTD6);
		power_timer0_disable();
	}
	else
	{
		power_timer0_enable();
		TCCR0A |= (1<<COM0A1);
		OCR0A = brightness;
	}
}
