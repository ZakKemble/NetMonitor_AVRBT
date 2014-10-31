/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "typedefs.h"
#include "config.h"

// Linked list for storing key/value pairs
typedef struct keyVal_s keyVal_s;
struct keyVal_s {
	keyVal_s* next;
	char* key;
	char* val;
};

static keyVal_s* keyValList;

static bool loadConfig(const char*);
static void add(char*, char*);

void config_init(const char* file)
{
	if(!loadConfig(file))
	{
		fprintf(stderr, "Error loading config file %s\n", file);
		exit(EXIT_FAILURE);
	}
}

// Delete everything
void config_end()
{
	keyVal_s* keyVal = keyValList;
	while(keyVal != NULL)
	{
		if(keyVal->key)
			free(keyVal->key);
		if(keyVal->val)
			free(keyVal->val);
		keyVal_s* temp = keyVal;
		keyVal = keyVal->next;
		free(temp);
	}
}

// Search for a key and get its value
void config_val(char* key, char** val)
{
	keyVal_s* keyVal = keyValList;
	while(keyVal != NULL)
	{
		if(strcmp(key, keyVal->key) == 0)
		{
			if(*val == NULL)
				*val = malloc(strlen(keyVal->val) + 1);
			strcpy(*val, keyVal->val);
			return;
		}
		keyVal = keyVal->next;
	}

	fprintf(stderr, "Error: Config option not found: %s\n", key);
	exit(EXIT_FAILURE);
}

// Add a new key value pair
static void add(char* key, char* val)
{
	keyVal_s* keyVal = keyValList;
	if(keyVal == NULL)
	{
		keyValList = malloc(sizeof(keyVal_s));
		keyVal = keyValList;
	}
	else
	{
		while(keyVal->next != NULL)
			keyVal = keyVal->next;

		keyVal->next = malloc(sizeof(keyVal_s));
		keyVal = keyVal->next;
	}

	keyVal->next = NULL;

	keyVal->key = malloc(strlen(key) + 1);
	strcpy(keyVal->key, key);

	keyVal->val = malloc(strlen(val) + 1);
	strcpy(keyVal->val, val);
}

// Read config file
static bool loadConfig(const char* file)
{
	// Open file
	FILE* fp = fopen(file, "r");
	if(!fp)
		return false;

	char* line = NULL;
	size_t len = 0;
	ssize_t read;

	// Once line at time
	while((read = getline(&line, &len, fp)) != -1)
	{
		// Make sure line isn't a comment or something
		if(
			read < 4 ||
			line[0] == 0x00 || // NULL
			line[0] == '#' ||
			line[0] == ' ' ||
			line[0] == '\n' ||
			line[0] == '\r')
		continue;

		// Look for '='
		for(int i=0;i<read;i++)
		{
			if(line[i] != '=')
				continue;

			// We found '=', so now we can get the key and value

			// Key
			size_t size = i;
			char* key = calloc(1, size + 1);
			memcpy(key, line, size);

			// Find the end
			int end = read;
			for(;end>i;end--)
			{
				if(
					line[end] != ' ' &&
					line[end] != '\n' &&
					line[end] != '\r' && 
					line[end] != 0x00 && // NULL
					line[end] != 0x0C && // Form feed thing... why did I put this here?
					line[end] != '\t' &&
					line[end] != 0x0B) // Vertical tab... still can't remember why...
				break;
			}

			// Value
			size = end - i;
			char* val = calloc(1, size + 1);
			memcpy(val, line + i + 1, size);

			//printf("%s (%d) --- %s (%d)\n", key, i + 1, val, size + 1);

			// Add to out list of pairs
			add(key, val);

			// Free memory
			free(key);
			free(val);

			break;
		}
	}

	if(line)
		free(line);

	// Close file
	fclose(fp);

	return true;
}
