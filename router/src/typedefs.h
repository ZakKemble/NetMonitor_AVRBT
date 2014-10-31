/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <curl/curl.h>
#include <regex.h>
#include <pthread.h>
#include <time.h>

#define UNUSED(x) (void)(x)

typedef uint8_t byte;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef struct {
	char* memory;
	size_t size;
}s_memoryChunk;

typedef struct {
	CURL* c;
	time_t lastUpdate;
	int interval;
	pthread_t hThread;
	bool threadBusy;
	regex_t reg;
	void* data;
	void (*dataReadyCallback)(s_memoryChunk*);
}s_updateData;

typedef struct {
	uint16_t up;
	uint16_t down;
}s_netUpDown;

#endif /* TYPEDEFS_H_ */