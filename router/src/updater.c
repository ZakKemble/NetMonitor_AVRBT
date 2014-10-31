/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <pthread.h>
#include <regex.h>
//#include <syslog.h>
#include "typedefs.h"
#include "updater.h"
#include "config.h"

#define CURL_TRIES		4
#define CURL_TIMEOUT	4

static void init(CURL**, const char*, const char*, const char*, regex_t*, const char*);
static void* thread(void*);
static size_t WriteMemoryCallback(char*, size_t, size_t, void*);

//static FILE* fp = NULL;

void updater_construct(s_updateData** uptData, char* cfgUrl, char* cfgUser, char* cfgPass, char* cfgRegex, char* cfgInterval, void (*dataReadyCallback)(s_memoryChunk*), size_t dataSize)
{
/*
	if(!fp)
	{
		fp = fopen("/btnetmon.log", "a+");
		char buff[] = "Start\n";
		fwrite(buff, 1, sizeof(buff) - 1, fp);
		fflush(fp);
	}
*/
	*uptData = calloc(1, sizeof(s_updateData));

	char* url = NULL;
	char* user = NULL;
	char* pass = NULL;
	char* regex = NULL;
	char* interval = NULL;

	// Get config
	config_val(cfgUrl, &url);
	config_val(cfgUser, &user);
	config_val(cfgPass, &pass);
	config_val(cfgRegex, &regex);
	config_val(cfgInterval, &interval);

	// Setup cURL and regex
	init(&(*uptData)->c, url, user, pass, &(*uptData)->reg, regex);

	// Set other stuff
	(*uptData)->dataReadyCallback = dataReadyCallback;
	(*uptData)->data = calloc(1, dataSize);
	(*uptData)->interval = atoi(interval);

	// Free memory
	free(url);
	free(user);
	free(pass);
	free(regex);
	free(interval);
}

static void init(CURL** c, const char* url, const char* user, const char* pass, regex_t* reg, const char* regex)
{
	// Setup cURL
	*c = curl_easy_init();
	curl_easy_setopt(*c, CURLOPT_URL, url);
	curl_easy_setopt(*c, CURLOPT_USERNAME, user);
	curl_easy_setopt(*c, CURLOPT_PASSWORD, pass);
	curl_easy_setopt(*c, CURLOPT_TIMEOUT, CURL_TIMEOUT);
	curl_easy_setopt(*c, CURLOPT_NOSIGNAL, 1); // Needed otherwise seg faults occur with multithreading
	curl_easy_setopt(*c, CURLOPT_WRITEFUNCTION, &WriteMemoryCallback);

	// Compile regex
	regcomp(reg, regex, REG_EXTENDED | REG_ICASE);
}

void updater_destroy(s_updateData** uptData)
{
	if((*uptData)->data)
		free((*uptData)->data);
	regfree(&(*uptData)->reg);
	curl_easy_cleanup((*uptData)->c);
	free(*uptData);
}

void updater_run(s_updateData* data)
{
	time_t now;
	time(&now);
	if(now - data->lastUpdate > data->interval && !data->threadBusy)
	{
		data->lastUpdate = now;
		data->threadBusy = true;
		pthread_create(&data->hThread, NULL, &thread, (void*)data);
	}
}

ulong updater_getMatchNum(regmatch_t* match, s_memoryChunk* chunk)
{
	size_t size = match->rm_eo - match->rm_so;
	char buff[size + 1];
	memcpy(buff, chunk->memory + match->rm_so, size);
	buff[size] = 0x00; // Make sure theres a NULL terminator
	return strtoul(buff, NULL, 10);
}

static void* thread(void* arg)
{
	pthread_detach(pthread_self());

	s_updateData* data = (s_updateData*)arg;

	s_memoryChunk chunk;
	chunk.memory = calloc(1, 1);
	chunk.size = 0;

	curl_easy_setopt(data->c, CURLOPT_WRITEDATA, (void*)&chunk);

	CURLcode res;

	for(byte i=0;i<CURL_TRIES;i++)
	{
		res = curl_easy_perform(data->c);

		if(res == CURLE_OK)
			break;

		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
//		syslog(LOG_DAEMON | LOG_WARNING, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
//		char buff[512];
//		int count = sprintf(buff, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
//		fwrite(buff, 1, count, fp);
//		fflush(fp);

		// Reset
		chunk.memory = realloc(chunk.memory, 1);
		chunk.size = 0;
		chunk.memory[0] = 0x00;
	}

	// Everything OK
	if(res == CURLE_OK)
		data->dataReadyCallback(&chunk);

	// Free memory
	if(chunk.memory)
		free(chunk.memory);

	// No longer busy
	data->threadBusy = false;

	return 0;
}

static size_t WriteMemoryCallback(char* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	s_memoryChunk* mem = (s_memoryChunk*)userp;

	mem->memory = realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL)
	{
		/* out of memory! */ 
		//printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0x00;

	return realsize;
}