/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

// Fuses for ATmega328/328P
// External full swing crystal 16K CK/14CK + 65ms startup
// Serial program (SPI) enabled
// Brown-out at 2.7V
// Bootloader enabled, 2048 words
// High:		0xD8 (or 0xDF for no bootloader)
// Low:			0xF7
// Extended:	0xFD

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

#define CFG_HISTORY_COUNT	40

// ADSL/PPPoE/PPPoA has a lot of overhead, this is the usable percentage, for calculating max rates
#define CFG_ADSL_USABLE		85

// Number of display modes
#define CFG_DISPLAY_MODES	2

// Show FPS (debugging)
#define CFG_FPS				0

// Serial baud rate for Bluetooth module
#define CFG_BAUD			38400

#endif /* CONFIG_H_ */