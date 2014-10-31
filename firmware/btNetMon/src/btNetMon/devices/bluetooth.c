/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "common.h"
#include "devices/bluetooth.h"
#include "drivers/uart.h"
#include "millis/millis.h"

// RST should only be input Hi-Z (normal operation) or output LOW (reset), never PULLUP or HIGH
// Bluetooth RST pin is pulled up to 1.8V, PULLUP/HIGH will apply 5V which will probably be bad

#define RST	D3
#define LED	C2
#define KEY	C0

#define SYNC_TO_USABLE(sync)	(sync * ((CFG_ADSL_USABLE / 100.0) / 8))
#define NETDATA_PACKET_SIZE		22
#define DATA_TIMEOUT			400

#define capVal(data, val) if(data > val){data = val;}

#define rstOn()		(pinMode(RST, OUTPUT))
#define rstOff()	(pinMode(RST, INPUT))

typedef struct {
	byte data[NETDATA_PACKET_SIZE];
	byte idx;
}s_packet;

static s_packet packet;

static void parsePacket(s_netData*, s_monitorData*);
static void readPacket(void*, byte);
static void setNetData(s_netData*, s_monitorData*);

void bluetooth_init()
{
	pinMode(KEY, OUTPUT);
	pinWrite(RST, LOW);
	rstOn();
	delay(100);
	rstOff();
}

// Read LED status
bool bluetooth_led()
{
	return pinRead(LED);
}

void bluetooth_update(s_monitorData* monitorData)
{
	static millis_t lastData;

	// Timeout, in case some bytes are lost so we can keep in sync
	millis_t now = millis();
	if(now - lastData > DATA_TIMEOUT)
	{
		lastData = 0;
		packet.idx = 0;
		memset(packet.data, 0, sizeof(packet.data));
	}

	byte b;

	// Get new data from UART buffer
	while(uart_get_nb(&b))
	{
		lastData = now;

		// Store in buffer
		packet.data[packet.idx++] = b;

		// Buffer full?
		if(packet.idx >= sizeof(packet.data))
		{
			packet.idx = 0;

			s_netData netData;
			parsePacket(&netData, monitorData);
			setNetData(&netData, monitorData);

			memset(packet.data, 0, sizeof(packet.data));
		}
	}
}

// Parse packet
static void parsePacket(s_netData* netData, s_monitorData* monitorData)
{
	// Read buffer data
	readPacket(&netData->sync.down,		2);
	readPacket(&netData->sync.up,		2);
	readPacket(&netData->rate.down,		2);
	readPacket(&netData->rate.up,		2);
	readPacket(&netData->ping,			2);
	readPacket(&netData->loss,			1);
	readPacket(&netData->ip,			4);
	readPacket(&monitorData->emails,	1);
	readPacket(&monitorData->time.hour,	1);
	readPacket(&monitorData->time.min,	1);
	readPacket(&monitorData->time.sec,	1);
	readPacket(&monitorData->time.date,	1);
	readPacket(&monitorData->time.mon,	1);
	readPacket(&monitorData->time.year,	1);

	// Make sure values are suitable
	capVal(netData->sync.down,		9999);
	capVal(netData->sync.up,		9999);
	capVal(netData->rate.down,		999);
	capVal(netData->rate.up,		999);
	capVal(netData->ping,			999);
	capVal(netData->loss,			99);
	capVal(monitorData->time.hour,	23);
	capVal(monitorData->time.min,	59);
	capVal(monitorData->time.sec,	59);
	capVal(monitorData->time.date,	31);
	capVal(monitorData->time.mon,	11);
	capVal(monitorData->time.year,	99);
}

static void readPacket(void* dst, byte len)
{
	memcpy(dst, packet.data + packet.idx, len);
	packet.idx += len;
}

// Set data
static void setNetData(s_netData* netData, s_monitorData* monitorData)
{
	// Set new data
	monitorData->netData.sync	= netData->sync;
	monitorData->netData.rate	= netData->rate;
	monitorData->netData.ping	= netData->ping;
	monitorData->netData.loss	= netData->loss;
	monitorData->netData.ip		= netData->ip;

	// Workout max down/up rates
	s_netUpDown maxRate;
	maxRate.down	= SYNC_TO_USABLE(netData->sync.down);
	maxRate.up		= SYNC_TO_USABLE(netData->sync.up);
	monitorData->netData.maxRate = maxRate;

	// Store history
	byte idx = monitorData->netHistoryIdx;
	memset(&monitorData->netHistory[idx], 0, sizeof(s_netHistory));
	if(maxRate.down > 0)
		monitorData->netHistory[idx].down	= remapInt(limitVal(netData->rate.down, 0, maxRate.down), 0, maxRate.down, 0, 255);
	if(maxRate.up > 0)
		monitorData->netHistory[idx].up		= remapInt(limitVal(netData->rate.up, 0, maxRate.up), 0, maxRate.up, 0, 255);
//	monitorData->netHistory[idx].ping		= netData->ping;
//	monitorData->netHistory[idx].loss		= netData->loss;
	if(++idx >= CFG_HISTORY_COUNT)
		idx = 0;
	monitorData->netHistoryIdx = idx;
}
