/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/
 */

// http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
// http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html

#define _BSD_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <pthread.h>
#include <oping.h>
#include <time.h>
#include <net/if.h>
#include <netdb.h>
#include "typedefs.h"
#include "ping.h"
#include "config.h"

static void* thread(void*);

static pingobj_t* oping;
static char currentHost[NI_MAXHOST] = "";
static bool* lostPackets;
static int lostPacketIdx;
static int lossTime;
static uint16_t ping;
static pthread_t hThread;

void ping_init()
{
	char* interface = NULL;
	char* strLossTime = NULL;

	config_val("interface", &interface);
	config_val("losstime", &strLossTime);

	lossTime = atoi(strLossTime);

	lostPackets = calloc(1, sizeof(bool) * lossTime);

	double val = 1;
	oping = ping_construct();
	ping_setopt(oping, PING_OPT_TIMEOUT, &val);
	ping_setopt(oping, PING_OPT_DEVICE, interface);
	//ping_setopt(oping, PING_OPT_SOURCE, "");

	if(interface)
		free(interface);
	if(strLossTime)
		free(strLossTime);

/*
	ping_send(oping);

	pingobj_iter_t *iter;
	for (iter = ping_iterator_get(oping); iter != NULL; iter = ping_iterator_next(iter))
	{
		double ping;
		size_t size = sizeof(ping);
		ping_iterator_get_info(iter, PING_INFO_LATENCY, &ping, &size);
		printf("Ping: %3.2f\n", ping);
		printf("%s\n", ping_get_error(oping));
	}
*/
	pthread_create(&hThread, NULL, &thread, NULL);
}

void ping_end()
{
	pthread_cancel(hThread); // MAKE SAFE!
	pthread_join(hThread, NULL);
	ping_destroy(oping);
	free(lostPackets);
}

uint16_t ping_get()
{
	return ping;
}

// Workout loss percentage
byte ping_loss()
{
	byte lostPacketCount = 0;
	for(byte i=0;i<lossTime;i++)
	{
		if(lostPackets[i])
			lostPacketCount++;
	}
	return (lostPacketCount / (float)lossTime) * 99;
}

// Set IP to ping
void ping_setHost(char* newHost)
{
	if(strcmp(newHost, currentHost) != 0)
	{
		ping_host_remove(oping, currentHost);
		ping_host_add(oping, newHost);
		strncpy(currentHost, newHost, NI_MAXHOST - 1);
	}
}

static void* thread(void* arg)
{
	UNUSED(arg);

	//pthread_detach(pthread_self());

	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	uint packetsLost = 0;
	pingobj_iter_t *iter;

	struct timespec sleepTime;
	sleepTime.tv_sec = 1;
	sleepTime.tv_nsec = 0;

	nanosleep(&sleepTime, NULL);

	while(1)
	{
		nanosleep(&sleepTime, NULL);

		// Time how long it takes to do the ping and everything, so pings happen exactly every 1 second
		struct timespec startTime;
		clock_gettime(CLOCK_REALTIME, &startTime);

		// Do ping
		if(ping_send(oping) < 0)
			fprintf(stderr, "%s\n", ping_get_error(oping));

		iter = ping_iterator_get(oping);

		if(iter != NULL)
		{
			uint32_t dropped;
			size_t size = sizeof(uint32_t);
			ping_iterator_get_info(iter, PING_INFO_DROPPED, &dropped, &size);

			if(dropped != packetsLost)
			{
				puts("Ping packet lost!");
				packetsLost = dropped;
				lostPackets[lostPacketIdx] = true;
			}
			else
				lostPackets[lostPacketIdx] = false;

			lostPacketIdx++;
			if(lostPacketIdx >= lossTime)
				lostPacketIdx = 0;

			double tempPing;
			size = sizeof(double);
			ping_iterator_get_info(iter, PING_INFO_LATENCY, &tempPing, &size);
			ping = tempPing;

//			int a;
//			size = sizeof(int);
//			ping_iterator_get_info(iter, PING_INFO_SEQUENCE, &a, &size);
//			printf("SEQ: %d\n", a);
		}
		else
		{
			puts("Ping packet lost!");
			lostPackets[lostPacketIdx] = true;
			lostPacketIdx++;
			if(lostPacketIdx >= lossTime)
				lostPacketIdx = 0;

			ping = -1;
		}

		struct timespec endTime;
		clock_gettime(CLOCK_REALTIME, &endTime);

		// Workout how long to wait until next ping
		if(endTime.tv_sec - startTime.tv_sec > 0)
		{
			sleepTime.tv_sec = 0;
			sleepTime.tv_nsec = 0;
		}
		else
		{
			sleepTime.tv_sec = 0;
			sleepTime.tv_nsec = 1000000000 - (endTime.tv_nsec - startTime.tv_nsec);
		}

		//printf("%ld\n", sleepTime.tv_nsec);
	}

	return 0;
}