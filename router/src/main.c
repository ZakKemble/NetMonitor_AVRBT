/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/
 */

// TODO:
// Keep byte count files always open?
// Instead of ping and main threads having their own sleep(), have a single sleep() which notifies other threads?
// If no responce from modem then sync speeds should be set to 0
// Support non-Point-to-Point interfaces

// BUGS:
// Sometimes cURL stuff stops working with 'Failure when receiving data from the peer'
// Sometimes program crashes when interface comes up
// Single instance stuff doesn't work yet
// Ping doesn't work if non-root, needs CAP_NET_RAW capability since liboping uses raw sockets
// If interface is unavailable ping is 999 (good), but lost packet count does not go up (bad)

#define _BSD_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <time.h>
#include <fcntl.h>
#include <stdarg.h>
//#include <syslog.h>
#include "typedefs.h"
#include "serial.h"
#include "adsl.h"
#include "email.h"
#include "ping.h"
#include "iface.h"
#include "config.h"

#define SENDBUFFER_SIZE	22

typedef struct{
	byte data[SENDBUFFER_SIZE];
	byte* ptr;
}s_sendBuffer;

// Byte count file locations
static char* byteCountFileRx;
static char* byteCountFileTx;

static bool alreadyRunning(void);
static void update(void);
static void bufferWriter(s_sendBuffer*, void*, byte);
static ulong readFileAsNum(const char*);
static int writeToBuffer(char**, const char*, ...);
static void print_ip(uint32_t);

int main(int argc, char **argv)
{
	puts("Bluetooth Net Monitor v1.0");
	puts("zakkemble.co.uk");

//	syslog(LOG_DAEMON | LOG_INFO, "Bluetooth Net Monitor v1.0 started");

	bool daemonize = true;
	char* configFile = NULL;

	// Check args
	int argIdx = 1;
	while(argIdx < argc)
	{
		if(strcmp(argv[argIdx], "-h") == 0 || strcmp(argv[argIdx], "--help") == 0)
		{
			printf("Usage: %s [--help|-h] [-c <file>] [-d]", argv[0]);
			puts("");
			puts(" --help|-h : Help (This)");
			puts(" -c <file> : Config file location (Default looks for btnetmon.conf in working directory)");
			puts("        -d : Don't daemonize (Debugging)");
			exit(EXIT_SUCCESS);
		}
		else if(strcmp(argv[argIdx], "-c") == 0)
		{
			argIdx++;
			writeToBuffer(&configFile, "%s", argv[argIdx]);
		}
		else if(strcmp(argv[argIdx], "-d") == 0)
			daemonize = false;
		argIdx++;
	}

	// Default config file
	if(!configFile)
		writeToBuffer(&configFile, "btnetmon.conf");

	// Load config
	config_init(configFile);

	// No longer needed
	if(configFile)
		free(configFile);

	// Make sure only 1 instance is running
	if(alreadyRunning())
	{
		fputs("Error: Another instance is already running", stderr);
		exit(EXIT_FAILURE);
	}

	// Run in background
	if(daemonize && daemon(0, 0) < 0)
		error(EXIT_FAILURE, errno, "Failed to daemonize");

	// Init
	curl_global_init(CURL_GLOBAL_ALL);
	serial_init();
	email_init();
	adsl_init();
	iface_init();
	ping_init();

	// Get byte count file locations
	char* interface = NULL;
	config_val("interface", &interface);

	writeToBuffer(&byteCountFileRx, "/sys/class/net/%s/statistics/rx_bytes", interface);
	writeToBuffer(&byteCountFileTx, "/sys/class/net/%s/statistics/tx_bytes", interface);

	if(interface)
		free(interface);

	// Free config resources
	config_end();

	struct timespec sleepTime;
	sleepTime.tv_sec = 1;
	sleepTime.tv_nsec = 0;

	//for(int i=0;i<5;i++)
	while(1)
	{
		nanosleep(&sleepTime, NULL);

		//struct timespec startTime;
		//clock_gettime(CLOCK_THREAD_CPUTIME_ID, &startTime);

		update();

		//struct timespec endTime;
		//clock_gettime(CLOCK_THREAD_CPUTIME_ID, &endTime);

		//printf("Total time: %ldns\n", endTime.tv_nsec - startTime.tv_nsec);
	}

	// Free everything, although program never ends unless its manually killed
	free(byteCountFileRx);
	free(byteCountFileTx);
	serial_end();
	iface_end();
	email_end();
	adsl_end();
	ping_end();
	curl_global_cleanup();

	return EXIT_SUCCESS;
}

// This isn't working quite right yet, file lock stuff doesn't lock...
static bool alreadyRunning()
{
	char* pid = NULL;
	config_val("pid", &pid);

	// If PID file setting is empty then don't do any PID stuff
	if(strlen(pid) < 1)
		return false;

	// Create/open file
	int pid_file = open(pid, O_CREAT | O_WRONLY | O_TRUNC, 0644);

	if(pid_file < 0)
	{
		perror("Unable to create PID file");
		exit(EXIT_FAILURE);
	}

	if(pid)
		free(pid);

	// Get PID
	char* buff = NULL;
	int len = writeToBuffer(&buff, "%d", getpid());

	// Write PID
	int res = write(pid_file, buff, len);
	UNUSED(res);

	if(buff)
		free(buff);

	// Try to lock file
	struct flock lock;
	lock.l_type		= F_WRLCK;
	lock.l_whence	= SEEK_SET;
	lock.l_start	= 0;
	lock.l_len		= 0;
	lock.l_pid		= getpid();
	int rc = fcntl(pid_file, F_SETLK, &lock);

	return (rc != 0);
}

// Main update function
static void update()
{
	static ulong lastBytesDown;
	static ulong lastBytesUp;

	// Start update threads and stuff
	email_update();
	adsl_update();
	iface_update();

	// Get what IP to ping
	ping_setHost(iface_gateway());

	// Get current byte count
	ulong bytesDown = readFileAsNum(byteCountFileRx);
	ulong bytesUp = readFileAsNum(byteCountFileTx);

	// Work out rates
	s_netUpDown rate;
	rate.down = (uint16_t)((bytesDown - lastBytesDown) / 1024);
	rate.up = (uint16_t)((bytesUp - lastBytesUp) / 1024);

	// Save byte counts for next time
	lastBytesDown = bytesDown;
	lastBytesUp = bytesUp;

	// IP
	uint32_t ip = iface_ip();

	// Time
	time_t rawTime;
	struct tm* timeInfo;
	time(&rawTime);
	timeInfo = localtime(&rawTime);

	// ADSL sync rates
	s_netUpDown adsl = adsl_get();

	// Email
	bool hasEmail = email_get();

	// Ping
	uint16_t ping = ping_get();

	// Ping loss
	byte loss = ping_loss();

	// Show info
	printf("Sync ---- Down: %hu -- Up: %hu\n", adsl.down, adsl.up);
	printf("Rates ---- Down: %hu -- Up: %hu\n", rate.down, rate.up);
	print_ip(ip);
	printf("Gateway: %s\n", iface_gateway());
	printf("Ping: %hu | Loss: %hhu\n", ping, loss);
	hasEmail ? puts("Email!") : puts("No email");
	printf("Time: %s\n", asctime(timeInfo));

	s_sendBuffer buffer;
	memset(&buffer.data, 0, sizeof(buffer.data));
	buffer.ptr = buffer.data;

	bufferWriter(&buffer, &adsl.down, 2);
	bufferWriter(&buffer, &adsl.up, 2);
	bufferWriter(&buffer, &rate.down, 2);
	bufferWriter(&buffer, &rate.up, 2);
	bufferWriter(&buffer, &ping, 2);
	bufferWriter(&buffer, &loss, 1);
	bufferWriter(&buffer, &ip, 4);
	bufferWriter(&buffer, &hasEmail, 1);
	bufferWriter(&buffer, &timeInfo->tm_hour, 1);
	bufferWriter(&buffer, &timeInfo->tm_min, 1);
	bufferWriter(&buffer, &timeInfo->tm_sec, 1);
	bufferWriter(&buffer, &timeInfo->tm_mday, 1);
	bufferWriter(&buffer, &timeInfo->tm_mon, 1);
	bufferWriter(&buffer, &timeInfo->tm_year, 1);

	for(size_t i=0;i<sizeof(buffer.data);i++)
		printf("%02x ", buffer.data[i]);
	puts("");

	puts("----");

	// Send to the bluetooth net monitor
	serial_send(buffer.data, sizeof(buffer.data));
}

static void bufferWriter(s_sendBuffer* buffer, void* src, byte len)
{
	memcpy(buffer->ptr, src, len);
	buffer->ptr += len;
}

// Read a file and convert its contents to ulong
static ulong readFileAsNum(const char* file)
{
	// Open file
	FILE* fp = fopen(file, "r");
	if(!fp)
		return 0;

	// Make buffer
	char buffer[21];
	memset(buffer, 0, sizeof(buffer));

	// Read contents
	int res = fread(buffer, sizeof(char), sizeof(buffer) - 1, fp);
	UNUSED(res);

	// Convert to ulong
	ulong num = strtoul(buffer, NULL, 10);

	// Close file
	fclose(fp);

	return num;
}

static int writeToBuffer(char** buff, const char* fmt, ...)
{
	va_list args1;
	va_list args2; // Using the same va_list in more than 1 place messes things up?
	va_start(args1, fmt);
	va_start(args2, fmt);

	// Get required buffer size
	int len = vsnprintf(NULL, 0, fmt, args1) + 1;

	if(*buff) // realloc instead?
		free(*buff);
	*buff = malloc(len);

	vsnprintf(*buff, len, fmt, args2);

	va_end(args1);
	va_end(args2);

//	printf("%d %s\n", len, *buff);
	return len;
}

// Print Int32 IP
static void print_ip(uint32_t ip)
{
	byte octects[4];
	octects[0] = ip & 0xFF;
	octects[1] = (ip >> 8) & 0xFF;
	octects[2] = (ip >> 16) & 0xFF;
	octects[3] = (ip >> 24) & 0xFF;	
	printf("IP: %hhu.%hhu.%hhu.%hhu\n", octects[0], octects[1], octects[2], octects[3]);        
}
