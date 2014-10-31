/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

#include "common.h"
#include "bluetooth.h"
#include "uart.h"
#include <string.h>
#include <stdlib.h>

#define SYNC_TO_USABLE(sync)	(sync * ((CFG_ADSL_USABLE / 100.0) / 8))
#define PACKET_STATE_NONE		0
#define PACKET_STATE_COMPLETE	1
#define NETDATA_PACKET_SIZE		71
#define CHAR_START				'$'
#define CHAR_END				'*'
#define CHAR_SPLIT				"|"
#define CHAR_START2				"$"
#define PACKET_POS_SYNCDOWN		1
#define PACKET_POS_SYNCUP		2
#define PACKET_POS_RATEDOWN		3
#define PACKET_POS_RATEUP		4
#define PACKET_POS_PING			5
#define PACKET_POS_LOSS			6
#define PACKET_POS_IP			7
#define PACKET_POS_EMAILS		8
#define PACKET_POS_DATE			9
#define PACKET_POS_TIME			10
/*
#define PACKET_POS_DAY			9
#define PACKET_POS_MONTH		10
#define PACKET_POS_DATE			11
#define PACKET_POS_YEAR			12
#define PACKET_POS_HOUR			13
#define PACKET_POS_MIN			14
#define PACKET_POS_SEC			15
*/

static char packetData[NETDATA_PACKET_SIZE];

static void parsePacket(s_netData*, s_monitorData*);
static void setNetData(s_netData*, s_monitorData*);

void bluetooth_tick(s_monitorData* monitorData)
{
	char* c = 0;
	static byte state = PACKET_STATE_NONE;
	static byte buffPtr = 0;
	
	// Get new data from UART buffer
	while(uart_get_nb((byte*)c))
	{
		switch(state)
		{
			case PACKET_STATE_NONE:
			{
				// New packet
				if(*c == CHAR_START)
				{
					buffPtr = 0;
					memset(packetData, 0, NETDATA_PACKET_SIZE);
				}
				
				if(*c == CHAR_END) // End of packet
					state = PACKET_STATE_COMPLETE;				
				else if(buffPtr < NETDATA_PACKET_SIZE) // Packet data
					packetData[buffPtr++] = *c;
			}
			break;
			default:
				break;
		}
	}

	// Got all packet data
	if(state == PACKET_STATE_COMPLETE)
	{
		buffPtr = 0;
		state = PACKET_STATE_NONE;
		s_netData netData = (s_netData){(s_netUpDown){0,0},(s_netUpDown){0,0},(s_netUpDown){0,0},0,0,"0.0.0.0"};
		parsePacket(&netData, monitorData);
		setNetData(&netData, monitorData);
		memset(packetData, 0, NETDATA_PACKET_SIZE);
	}
}

// Parse packet text data
static void parsePacket(s_netData* netData, s_monitorData* monitorData)
{
	byte count = 0;
	char* token = strtok(packetData,CHAR_SPLIT);
	if(token != NULL && strcmp(token,CHAR_START2) == 0)
	{
		while(token != NULL)
		{
			switch(count)
			{
				case PACKET_POS_SYNCDOWN:
					netData->sync.down	= atoi(token);
					break;
				case PACKET_POS_SYNCUP:
					netData->sync.up	= atoi(token);
					break;
				case PACKET_POS_RATEDOWN:
					netData->rate.down	= atoi(token);
					break;
				case PACKET_POS_RATEUP:
					netData->rate.up	= atoi(token);
					break;
				case PACKET_POS_PING:
					netData->ping		= atoi(token);
					break;
				case PACKET_POS_LOSS:
					netData->loss		= atoi(token);
					break;
				case PACKET_POS_IP:
					memccpy(netData->ip, token, 0x00, 15);
					break;
				case PACKET_POS_EMAILS:
					monitorData->emails	= atoi(token);
					break;
				case PACKET_POS_DATE:
					memccpy(monitorData->time.date, token, 0x00, 13);
					break;
				case PACKET_POS_TIME:
					memccpy(monitorData->time.time, token, 0x00, 8);
					break;
				/*
				case PACKET_POS_DAY:
					memccpy(monitorData->time.day, token, 0x00, 3);
					break;
				case PACKET_POS_MONTH:
					memccpy(monitorData->time.month, token, 0x00, 3);
					break;
				case PACKET_POS_DATE:
					monitorData->time.date		= atoi(token);
					break;
				case PACKET_POS_YEAR:
					monitorData->time.year		= atoi(token);
					break;
				case PACKET_POS_HOUR:
					monitorData->time.hour		= atoi(token);
					break;
				case PACKET_POS_MIN:
					monitorData->time.minute	= atoi(token);
					break;
				case PACKET_POS_SEC:
					monitorData->time.seconds	= atoi(token);
					break;
				*/
				default:
					break;
			}
			++count;
			token = strtok(NULL,CHAR_SPLIT);
		}
	}
}

// Set data
static void setNetData(s_netData* netData, s_monitorData* monitorData)
{
	// Set new data
	monitorData->netData.sync = netData->sync;
	monitorData->netData.rate = netData->rate;
	monitorData->netData.ping = netData->ping;
	monitorData->netData.loss = netData->loss;
	memccpy(monitorData->netData.ip, netData->ip, 0x00, 15);
	
	// Workout max down/up rates
	s_netUpDown maxRate;
	maxRate.down = SYNC_TO_USABLE(netData->sync.down);
	maxRate.up = SYNC_TO_USABLE(netData->sync.up);
	monitorData->netData.maxRate = maxRate;
	
	// Store history
	byte idx = monitorData->netHistoryIdx;
	monitorData->netHistory[idx].down = mapInt(constrain(netData->rate.down, 0, maxRate.down),0,maxRate.down,255,0);
	monitorData->netHistory[idx].up = mapInt(constrain(netData->rate.up, 0, maxRate.up),0,maxRate.up,255,0);
	monitorData->netHistory[idx].ping = netData->ping;
	monitorData->netHistory[idx].loss = netData->loss;
	idx++;
	if(idx > CFG_HISTORY_COUNT - 1)
		idx = 0;
	monitorData->netHistoryIdx = idx;
}
