/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t byte;
typedef uint32_t ulong;
typedef uint16_t uint;

typedef struct{
	uint8_t whole;
	uint8_t frac;
}wholeFrac_s;

typedef struct{
	uint32_t up;
	uint32_t down;
}s_netUpDown;

typedef struct{
	s_netUpDown sync;
	s_netUpDown rate;
	s_netUpDown maxRate;
	uint16_t ping;
	uint8_t loss;
	uint32_t ip;
}s_netData;

typedef struct{
	uint8_t down;
	uint8_t up;
//	byte ping;
//	byte loss;
}s_netHistory;

typedef struct{
	uint8_t width;
	uint8_t height;
	uint16_t size;
	uint8_t offset;
	const uint8_t* font;
}s_font;

typedef struct{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}s_rgb;

typedef struct{
	s_rgb precision;
	s_rgb foreground;
//	s_rgb background;
}s_colour;

typedef union{
	uint16_t word;
	byte bytes[2];
}s_pixel;

typedef struct{
	byte date;
	byte mon;
	byte year;
	byte hour;
	byte min;
	byte sec;
}s_time;

typedef struct{
	s_netData netData;
	s_netHistory netHistory[CFG_HISTORY_COUNT];
	byte netHistoryIdx;
	byte emails;
	s_time time;
}s_monitorData;

typedef void (*f_displayStaticFunc)(void);
typedef void (*f_displayFunc)(s_monitorData*);

#endif /* TYPEDEFS_H_ */