/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

#ifndef UART_H_
#define UART_H_

#include "typedefs.h"

void uart_init();
bool uart_get_nb(byte*);

#endif /* UART_H_ */