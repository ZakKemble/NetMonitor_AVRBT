/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#ifndef LM73_DEFS_H_
#define LM73_DEFS_H_

// I2C addresses
#define LM73I2C_W			0b10010000
#define LM73I2C_R			0b10010001

// Registers
#define LM73ADDR_TEMP		0x00
#define LM73ADDR_CONF		0x01
#define LM73ADDR_THIGH		0x02
#define LM73ADDR_TLOW		0x03
#define LM73ADDR_CTRL		0x04
#define LM73ADDR_ID			0x07

// Configuration bits
#define LM73BIT_PWRDOWN	7
#define LM73BIT_ALRTEN	5
#define LM73BIT_ALRTPOL	4
#define LM73BIT_ALRTRST	3
#define LM73BIT_ONESHOT	2

// Control/status bits
#define LM73BIT_TIMEOUT		7
#define LM73BIT_RESOLUTION1	6
#define LM73BIT_RESOLUTION0	5
#define LM73BIT_ALRTSTATUS	3
#define LM73BIT_THIGHFLAG	2
#define LM73BIT_TLOWFALG	1
#define LM73BIT_DATAREADY	0

#endif /* LM73_DEFS_H_ */