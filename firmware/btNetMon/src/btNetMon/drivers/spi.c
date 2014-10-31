/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#include "common.h"

#define MISO	B4
#define MOSI	B3
#define SCK		B5
#define SS		B2

void spi_init()
{
	pinMode(SS, OUTPUT);
	pinMode(MOSI, OUTPUT);
	pinMode(SCK, OUTPUT);
	pinPullup(MISO, PU_EN);
	pinWrite(SS, HIGH);

	SPCR = _BV(SPE)|_BV(MSTR)|_BV(CPOL)|_BV(CPHA);
//	SPSR = _BV(SPI2X);
}

