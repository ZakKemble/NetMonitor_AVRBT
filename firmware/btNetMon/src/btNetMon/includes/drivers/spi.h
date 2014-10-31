/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>
#include "common.h"

void spi_init(void);

inline byte spi_transfer(byte data)
{
	SPDR = data;
	loop_until_bit_is_set(SPSR, SPIF);
	return SPDR;
}

inline void spi_transfer_nr(byte data)
{
	SPDR = data;
	loop_until_bit_is_set(SPSR, SPIF);
}

#endif /* SPI_H_ */