/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

#ifndef SPI_H_
#define SPI_H_

#include "common.h"
#include <avr/io.h>

#define SPI_MOSI_DDR	DDR(CFG_SPI_MOSI_PORT)
#define SPI_MOSI_PORT	PORT(CFG_SPI_MOSI_PORT)
#define SPI_MOSI_BIT	PORTBIT(CFG_SPI_MOSI_PORT, CFG_SPI_MOSI_BIT)
#define SPI_MISO_DDR	DDR(CFG_SPI_MISO_PORT)
#define SPI_MISO_PIN	PORT(CFG_SPI_MISO_PORT)
#define SPI_MISO_BIT	PORTBIT(CFG_SPI_MISO_PORT, CFG_SPI_MISO_BIT)
#define SPI_SCK_DDR		DDR(CFG_SPI_SCK_PORT)
#define SPI_SCK_PORT	PORT(CFG_SPI_SCK_PORT)
#define SPI_SCK_BIT		PORTBIT(CFG_SPI_SCK_PORT, CFG_SPI_SCK_BIT)

#define SPI_HARDWARE	CFG_SPI_HARDWARE

void spi_init();

byte inline spi_transfer(byte data)
{
#if SPI_HARDWARE
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
#else
	byte dataIn;
	for(byte i=0;i<8;i++)
	{
		if(data & 0b10000000)
			SPI_MOSI_PORT |= (1<<SPI_MOSI_BIT);
		else
			SPI_MOSI_PORT &= ~(1<<SPI_MOSI_BIT);
		SPI_SCK_PORT |= (1<<SPI_SCK_BIT);
		dataIn <<= 1;
		if(SPI_MISO_PIN & (1<<SPI_MISO_BIT))
			dataIn |= 1;
		SPI_SCK_PORT &= ~(1<<SPI_SCK_BIT);
		data <<= 1;
	}
	return dataIn;
#endif
}

#endif /* SPI_H_ */