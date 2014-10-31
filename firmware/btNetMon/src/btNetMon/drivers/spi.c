/*

#include <avr/io.h>
#include "common.h"
#include "drivers/spi.h"

#define MISO	B4
#define MOSI	B3
#define SCK		B5
#define SS		B2

void spi_init()
{
	pinMode(SS, OUTPUT);
	pinMode(MOSI, OUTPUT);
	pinMode(SCK, OUTPUT);
	pinPullup(MISO, PULLUP_ENABLE);
	pinWrite(SS, HIGH);

	SPCR = _BV(SPE)|_BV(MSTR)|_BV(CPOL)|_BV(CPHA);
	SPSR = _BV(SPI2X);
}
