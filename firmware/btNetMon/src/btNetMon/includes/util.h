/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#ifndef UTIL_H_
#define UTIL_H_

#define UNUSED(var) ((void)(var))
#define EMPTY_FUNC	((void)(0))

#define noinline __attribute__ ((__noinline__))

#define delay(ms) _delay_ms(ms)
#define delay_us(us) _delay_us(us)

#define CONCAT(a, b) a ## b

#define B0	B, 0
#define B1	B, 1
#define B2	B, 2
#define B3	B, 3
#define B4	B, 4
#define B5	B, 5
#define B6	B, 6
#define B7	B, 7
#define C0	C, 0
#define C1	C, 1
#define C2	C, 2
#define C3	C, 3
#define C4	C, 4
#define C5	C, 5
#define C6	C, 6
#define C7	C, 7
#define D0	D, 0
#define D1	D, 1
#define D2	D, 2
#define D3	D, 3
#define D4	D, 4
#define D5	D, 5
#define D6	D, 6
#define D7	D, 7

#define bit_set_concat(reg, reg2, bit)		(CONCAT(reg, reg2) |= _BV(bit))
#define bit_clr_concat(reg, reg2, bit)		(CONCAT(reg, reg2) &= ~_BV(bit))
#define bit_tog_concat(reg, reg2, bit)		(CONCAT(reg, reg2) ^= _BV(bit))
#define bit_is_set_concat(reg, reg2, bit)	(bit_is_set(CONCAT(reg, reg2), bit))

#define HIGH			bit_set_concat
#define LOW				bit_clr_concat
#define TOGGLE			bit_tog_concat
#define pinWrite(x, s)	s(PORT, x)

#define OUTPUT			bit_set_concat
#define INPUT			bit_clr_concat
#define pinMode(x, s)	s(DDR, x)

#define PU_EN			bit_set_concat
#define PU_DIS			bit_clr_concat
#define pinPullup(x, s) s(PORT, x)

#define PINHIGH			true
#define PINLOW			false
#define pinRead(x)		bit_is_set_concat(PIN, x)

#define bits_set(data, bits)	((data) |= (bits))
#define bits_clr(data, bits)	((data) &= (~(bits)))
#define bit_set(data, x)		bits_set((data), _BV(x))
#define bit_clr(data, x)		bits_clr((data), _BV(x))

#endif /* UTIL_H_ */