/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

/*
 * Copyright (C) <year> Rossum
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>
#include "common.h"
#include "devices/lcd.h"
#include "devices/lcd_defs.h"
#include "drivers/spi.h"

#define BKL	D6
#define RST	D7

#define rstHigh()		(pinWrite(RST, HIGH))
#define rstLow()		(pinWrite(RST, LOW))

void lcd_init()
{
	// Timer 0 for LCD brightness
	TCCR0A = _BV(WGM00);
	TCCR0B = _BV(CS00)|_BV(CS01);

	pinMode(BKL, OUTPUT);

	pinMode(RST, OUTPUT);
	pinMode(CS, OUTPUT);
	pinMode(RS, OUTPUT);

	rstHigh();
	cmdMode();
	spiDeselect();
	
	delay(120);
	rstLow();
	delay(10);
	rstHigh();

// Init code by Rossum, MIT licence
	delay(120);
	lcd_writeCommand(ST7735_SWRESET); // software reset
	delay(150);

	lcd_writeCommand(ST7735_SLPOUT);  // out of sleep mode
	delay(100);

	lcd_writeCommand(ST7735_FRMCTR1);  // frame rate control - normal mode
	lcd_writeData(0x01);  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
	lcd_writeData(0x2C);
	lcd_writeData(0x2D);

	lcd_writeCommand(ST7735_FRMCTR2);  // frame rate control - idle mode
	lcd_writeData(0x01);  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
	lcd_writeData(0x2C);
	lcd_writeData(0x2D);

	lcd_writeCommand(ST7735_FRMCTR3);  // frame rate control - partial mode
	lcd_writeData(0x01); // dot inversion mode
	lcd_writeData(0x2C);
	lcd_writeData(0x2D);
	lcd_writeData(0x01); // line inversion mode
	lcd_writeData(0x2C);
	lcd_writeData(0x2D);
  
	lcd_writeCommand(ST7735_INVCTR);  // display inversion control
	lcd_writeData(0x07);  // no inversion

	lcd_writeCommand(ST7735_PWCTR1);  // power control
	lcd_writeData(0xA2);
	lcd_writeData(0x02);      // -4.6V
	lcd_writeData(0x84);      // AUTO mode

	lcd_writeCommand(ST7735_PWCTR2);  // power control
	lcd_writeData(0xC5);      // VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD

	lcd_writeCommand(ST7735_PWCTR3);  // power control
	lcd_writeData(0x0A);      // Opamp current small
	lcd_writeData(0x00);      // Boost frequency

	lcd_writeCommand(ST7735_PWCTR4);  // power control
	lcd_writeData(0x8A);      // BCLK/2, Opamp current small & Medium low
	lcd_writeData(0x2A);

	lcd_writeCommand(ST7735_PWCTR5);  // power control
	lcd_writeData(0x8A);
	lcd_writeData(0xEE);

	lcd_writeCommand(ST7735_VMCTR1);  // power control
	lcd_writeData(0x0E);

	lcd_writeCommand(ST7735_INVOFF);    // don't invert display

	lcd_writeCommand(ST7735_MADCTL);  // memory access control (directions)
	lcd_writeData(ROTATION_NORMAL);  // row address/col address, bottom to top refresh

	lcd_writeCommand(ST7735_COLMOD);  // set color mode
	lcd_writeData(LCD_COL_MODE);        // 16-bit color

	lcd_writeCommand(ST7735_CASET);  // column addr set
	lcd_writeData(0x00);
	lcd_writeData(0x00);   // XSTART = 0
	lcd_writeData(0x00);
	lcd_writeData(0x7F);   // XEND = 127

	lcd_writeCommand(ST7735_RASET);  // row addr set
	lcd_writeData(0x00);
	lcd_writeData(0x00);    // XSTART = 0
	lcd_writeData(0x00);
	lcd_writeData(0x9F);    // XEND = 159

	lcd_writeCommand(ST7735_GMCTRP1);
	lcd_writeData(0x0f);
	lcd_writeData(0x1a);
	lcd_writeData(0x0f);
	lcd_writeData(0x18);
	lcd_writeData(0x2f);
	lcd_writeData(0x28);
	lcd_writeData(0x20);
	lcd_writeData(0x22);
	lcd_writeData(0x1f);
	lcd_writeData(0x1b);
	lcd_writeData(0x23);
	lcd_writeData(0x37);
	lcd_writeData(0x00);
	lcd_writeData(0x07);
	lcd_writeData(0x02);
	lcd_writeData(0x10);
	lcd_writeCommand(ST7735_GMCTRN1);
	lcd_writeData(0x0f);
	lcd_writeData(0x1b);
	lcd_writeData(0x0f);
	lcd_writeData(0x17);
	lcd_writeData(0x33);
	lcd_writeData(0x2c);
	lcd_writeData(0x29);
	lcd_writeData(0x2e);
	lcd_writeData(0x30);
	lcd_writeData(0x30);
	lcd_writeData(0x39);
	lcd_writeData(0x3f);
	lcd_writeData(0x00);
	lcd_writeData(0x07);
	lcd_writeData(0x03);
	lcd_writeData(0x10);
  
	lcd_writeCommand(ST7735_DISPON);
	delay(100);

	lcd_writeCommand(ST7735_NORON);  // normal display on
	delay(10);
}

void lcd_writeCommand(byte data)
{
	LCD_SELECT(MODE_CMD)
	{
		spi_transfer_nr(data);
	}
}

void lcd_writeData(byte data)
{
	LCD_SELECT(MODE_DATA)
	{
		spi_transfer_nr(data);
	}
}

void lcd_setRotation(byte data)
{
	lcd_writeCommand(ST7735_MADCTL);  // memory access control (directions)
	lcd_writeData(data);  // row address/col address, bottom to top refresh
}

void lcd_setAddrWindow(byte x0, byte y0, byte x1, byte y1)
{
	lcd_writeCommand(ST7735_CASET);  // column addr set
	lcd_writeData(0x00);
	lcd_writeData(x0);   // XSTART
	lcd_writeData(0x00);
	lcd_writeData(x1);   // XEND

	lcd_writeCommand(ST7735_RASET);  // row addr set
	lcd_writeData(0x00);
	lcd_writeData(y0);    // YSTART
	lcd_writeData(0x00);
	lcd_writeData(y1);    // YEND

	lcd_writeCommand(ST7735_RAMWR);  // write to RAM
}

void lcd_blank()
{
	lcd_setAddrWindow(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
	LCD_SELECT(MODE_DATA)
	{
		for(byte y=LCD_HEIGHT;y--;)
		{
			for(byte x=LCD_WIDTH;x--;)
			{
				spi_transfer_nr(0);
				spi_transfer_nr(0);
			}
		}
	}
}

void lcd_setBrightness(byte brightness)
{
	if(brightness == 0 || brightness == 255)
	{
		TCCR0A &= ~_BV(COM0A1);
		if(brightness)
			pinWrite(BKL, HIGH);
		else
			pinWrite(BKL, LOW);
		power_timer0_disable();
	}
	else
	{
		power_timer0_enable();
		TCCR0A |= _BV(COM0A1);
		OCR0A = brightness;
	}
}
