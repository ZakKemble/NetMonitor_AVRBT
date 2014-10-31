/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

// Fuses for ATmega328/328P
// External crystal 8Mhz+ 16K CK/14CK + 65ms startup
// Serial program (SPI) enabled
// Brown-out at 4.3V
// High:		0xDF
// Low:			0xFF
// Extended:	0xFC

// Compiling
// For best results (size and speed) use the following settings for the compiler
// -mcall-prologues -funsigned-char -funsigned-bitfields -O2 -fno-tree-scev-cprop -ffreestanding -fpack-struct -fshort-enums -Wall -c -std=gnu99
//
// And these for the linker
// -mrelax
//
// Compiler: AVR-GCC 4.7.2

#ifndef CONFIG_H_
#define CONFIG_H_

#define F_CPU				20000000
#define CPU_DIV				clock_div_1

#define CFG_HISTORY_COUNT	40

// ADSL/PPPoE/PPPoA has a lot of overhead, this is the usable percentage, for calculating max rates
#define CFG_ADSL_USABLE		85

// Number of display modes
#define CFG_DISPLAY_MODES	2

// Show FPS (debugging)
#define CFG_FPS				0

// Serial baud rate for Bluetooth module
#define CFG_BAUD			9600

// Use hardware SPI
#define CFG_SPI_HARDWARE	1

// Software SPI settings
#define CFG_SPI_MOSI_PORT	B
#define CFG_SPI_MOSI_BIT	3
#define CFG_SPI_MISO_PORT	B
#define CFG_SPI_MISO_BIT	4
#define CFG_SPI_SCK_PORT	B
#define CFG_SPI_SCK_BIT		5

// LCD settings
#define CFG_LCD_RST_PORT	D
#define CFG_LCD_RST_BIT		7
#define CFG_LCD_CS_PORT		B
#define CFG_LCD_CS_BIT		2
#define CFG_LCD_RS_PORT		B
#define CFG_LCD_RS_BIT		1

// Button settings
#define CFG_BTN_PORT		D
#define CFG_BTN_BIT			3

// 1 = 12bit
// 2 = 16bit
// 3 = 18bit
// Only 16bit is supported for this project
#define CFG_LCD_COLOUR_MODE 2

#endif /* CONFIG_H_ */