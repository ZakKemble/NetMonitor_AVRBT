/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include "config.h"
#include <stdbool.h>

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
	char ip[16];
}s_netData;

typedef struct{
	byte down;
	byte up;
	byte ping;
	byte loss;
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

typedef struct{
/*
	char day[4];
	char month[4];
	byte date;
	byte year;
	byte hour;
	byte minute;
	byte seconds;
*/
	char date[14];
	char time[9];
}s_time;

typedef struct{
	s_netData netData;
	s_netHistory netHistory[CFG_HISTORY_COUNT];
	byte netHistoryIdx;
	byte emails;
	s_time time;
}s_monitorData;

#endif /* TYPEDEFS_H_ */