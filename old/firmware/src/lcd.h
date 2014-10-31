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

#ifndef LCD_H_
#define LCD_H_

#include "common.h"

#define ST7735_NOP 0x0
#define ST7735_SWRESET 0x01
#define ST7735_RDDID 0x04
#define ST7735_RDDST 0x09

#define ST7735_SLPIN  0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON  0x12
#define ST7735_NORON  0x13

#define ST7735_INVOFF 0x20
#define ST7735_INVON 0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON 0x29
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E

#define ST7735_COLMOD 0x3A
#define ST7735_MADCTL 0x36


#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_RDID1 0xDA
#define ST7735_RDID2 0xDB
#define ST7735_RDID3 0xDC
#define ST7735_RDID4 0xDD

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define LCD_RST_DDR	DDR(CFG_LCD_RST_PORT)
#define LCD_RST_PORT	PORT(CFG_LCD_RST_PORT)
#define LCD_RST_BIT	PORTBIT(CFG_LCD_RST_PORT, CFG_LCD_RST_BIT)
#define LCD_CS_DDR	DDR(CFG_LCD_CS_PORT)
#define LCD_CS_PORT	PORT(CFG_LCD_CS_PORT)
#define LCD_CS_BIT	PORTBIT(CFG_LCD_CS_PORT, CFG_LCD_CS_BIT)
#define LCD_RS_DDR	DDR(CFG_LCD_RS_PORT)
#define LCD_RS_PORT	PORT(CFG_LCD_RS_PORT)
#define LCD_RS_BIT	PORTBIT(CFG_LCD_RS_PORT, CFG_LCD_RS_BIT)

#if CFG_LCD_COLOUR_MODE == 1
#define LCD_COL_MODE 0x03
#elif CFG_LCD_COLOUR_MODE == 2
#define LCD_COL_MODE 0x05
#elif CFG_LCD_COLOUR_MODE == 3
#define LCD_COL_MODE 0x06
#else
#error "Invalid CFG_LCD_COLOUR_MODE"
#define LCD_COL_MODE 0x05
#endif

#define LCD_WIDTH 160
#define LCD_HEIGHT 128

void lcd_init();
void lcd_writecommand(byte);
void lcd_writedata(byte);
void lcd_setRotation(byte);
void lcd_setAddrWindow(byte, byte, byte, byte);
void lcd_blank();
void lcd_setBrightness(byte);

#endif /* LCD_H_ */
