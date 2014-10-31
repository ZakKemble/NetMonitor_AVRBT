/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2014 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/
 */

#ifndef COMMON_H_
#define COMMON_H_

#define _GNU_SOURCE
#define _BSD_SOURCE
#define _POSIX_C_SOURCE 200809L

//#include <syslog.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <regex.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <oping.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <curl/curl.h>
#include <net/if.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "typedefs.h"
#include "serial.h"
#include "modem.h"
#include "email.h"
#include "ping.h"
#include "iface.h"
#include "config.h"
#include "rfcomm.h"
#include "updater.h"

#endif /* COMMON_H_ */