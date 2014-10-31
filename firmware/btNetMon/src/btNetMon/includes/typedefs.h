/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdbool.h>
#include <stdint.h>
#include "config.h"

typedef unsigned char byte;
typedef unsigned long ulong;
typedef unsigned int uint;

typedef struct{
	uint up;
	uint down;
}s_netUpDown;

typedef struct{
	s_netUpDown sync;
	s_netUpDown rate;
	s_netUpDown maxRate;
	uint ping;
	byte loss;
	uint32_t ip;
}s_netData;

typedef struct{
	byte down;
	byte up;
//	byte ping;
//	byte loss;
}s_netHistory;

typedef struct{
	byte width;
	byte height;
	uint size;
	byte offset;
	const byte* font;
}s_font;

typedef struct{
	byte r;
	byte g;
	byte b;
}s_rgb;

typedef struct{
	s_rgb precision;
	s_rgb foreground;
//	s_rgb background;
}s_colour;

typedef union{
	uint word;
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