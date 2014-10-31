/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#define PRECISION 64

// Large fonts
#define COL_COL1 2,223,43

// Small fonts
#define COL_COL2 23,184,231

// Usage bar
#define COL_COL3 231,184,23

// Graph border
#define COL_GRAPH_BORDER 0,128,255

// Up rate history bars
#define COL_BAR_UP 255,128,0

// Down rate history bars
#define COL_BAR_DOWN 0,128,255

// Usage bar background
#define COL_USAGE_BG 92,128,92

#define STR_Kbps	0
#define STR_KB		1
#define STR_PS		2
#define STR_MS		3
#define STR_PERC	4
#define STR_KBps	5

void display_setMode(byte);
void display_drawDynamic(s_monitorData*);

#endif /* DISPLAY_H_ */