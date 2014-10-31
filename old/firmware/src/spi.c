/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

#include "common.h"
#include "spi.h"
#include <avr/io.h>

void spi_init()
{
#if SPI_HARDWARE
	DDRB |= (1<<DDB2)|(1<<DDB3)|(1<<DDB5);
	PORTB |= (1<<PORTB4)|(1<<PORTB2);
	PORTB &= ~((1<<PORTB3)|(1<<PORTB5));

	SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA);
	SPSR = (1<<SPI2X);
#else
	SPI_MOSI_DDR |= (1<<SPI_MOSI_BIT);
	SPI_MISO_DDR &= ~(1<<SPI_MISO_BIT);
	SPI_SCK_DDR |= (1<<SPI_SCK_BIT);
#endif
}
