/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

#ifndef UTIL_H_
#define UTIL_H_

#include "typedefs.h"

#define noinline __attribute__ ((__noinline__))

#define delay(ms) _delay_ms(ms)
#define delay_us(us) _delay_us(us)

#define CONCAT(a, b) a ## b
#define CONCAT2(a, b, c) a ## b ## c

#define PORT(port)			CONCAT(PORT, port)			// PORTB
#define PORTBIT(port, bit)	CONCAT2(PORT, port, bit)	// PORTB2
#define DDR(port)			CONCAT(DDR, port)			// DDRB
#define PINPORT(port)		CONCAT(PIN, port)			// PINB
#define PINBIT(port, bit)	CONCAT2(PIN, port, bit)		// PINB2
#define PCINT(pcint)		CONCAT(PCINT, pcint)		// PCINT1

void util_init();
ulong millis();
byte map(byte, byte, byte, byte, byte);
int mapInt(int, int, int, int, int);
long constrain(long, long, long);

#endif /* UTIL_H_ */