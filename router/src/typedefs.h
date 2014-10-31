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

#define UNUSED(x) (void)(x)

typedef uint8_t byte;
typedef unsigned int uint;
typedef unsigned long ulong;

// cURL Received data from web request
typedef struct {
	char* memory;
	size_t size;
}s_memoryChunk;

typedef struct {
	CURL* c;			// cURL object for web request
	time_t lastUpdate;	// Time of last update
	int interval;		// How often to update (seconds)
	pthread_t hThread;	// The thread doing the update
	bool threadBusy;	// Update thread is running
	regex_t reg;		// Regular expression for parsing the received data
	void* data;			// Pointer for storing the parsed data
	void (*dataReadyCallback)(s_memoryChunk*);	// Received data for parsing
}s_updateData;

typedef struct {
	uint32_t up;
	uint32_t down;
}s_netUpDown;

#endif /* TYPEDEFS_H_ */