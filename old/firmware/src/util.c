/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

#include "common.h"
#include <util/atomic.h>

#if F_CPU > 16320000
	#define CLOCKSEL (1<<CS22)|(1<<CS20)
	#define TICKS ((F_CPU / 128) / 1000)
#elif F_CPU >= 64000
	#define CLOCKSEL (1<<CS22)
	#define TICKS ((F_CPU / 64) / 1000)
#else
	#error "F_CPU too low (< 64000), add more clock selects"
#endif

static volatile ulong milliseconds = 0;

void util_init()
{
	// Timer 2 settings for approx. millisecond tracking
	TCCR2A = (1<<WGM21);
	TCCR2B = CLOCKSEL;
	TIMSK2 = (1<<OCIE2A);
	OCR2A = TICKS;
}

ulong millis()
{
	ulong ms;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		ms = milliseconds;
	}
	return ms;
}

// This is the map function from Arduino, modified for byte values instead of long
byte map(byte x, byte in_min, byte in_max, byte out_min, byte out_max)
{
	// At least one of the values must be 32 bit, in this case x is type casted to long since some parts of the calculation can return values down to -65025.
	// Or something like that... without the type cast the return value isn't quite right, map(255,0,255,0,255) would return 254

	return (((long)x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

int mapInt(int x, int in_min, int in_max, int out_min, int out_max)
{
	return (((long)x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

// This is the constrain function from Arduino
long constrain(long val, long min, long max)
{
	if(val < min)
		return min;
	else if(val > max)
		return max;
	else
		return val;
}

ISR(TIMER2_COMPA_vect)
{
	++milliseconds;
}
