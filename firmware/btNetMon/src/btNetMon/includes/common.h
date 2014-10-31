/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#ifndef COMMON_H_
#define COMMON_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/twi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "config.h"
#include "util.h"
#include "typedefs.h"

// MOVE THESE
byte remap(byte, byte, byte, byte, byte);
int remapInt(int, int, int, int, int);
int32_t remap32(int32_t, int32_t, int32_t, int32_t, int32_t);
uint limitVal(uint, uint, uint);
uint32_t limitVal32(uint32_t, uint32_t, uint32_t);

#include "devices/lcd.h"
#include "devices/bluetooth.h"
#include "devices/buttons.h"
#include "devices/lm73.h"

#include "drivers/spi.h"
#include "drivers/uart.h"
#include "drivers/i2c.h"

#include "millis/millis.h"

#include "views/mode1.h"
#include "views/mode2.h"

#include "btNetMon.h"
#include "draw.h"
#include "display.h"
#include "usb.h"

#include "colours.h"
#include "resources.h"

#include "../usbdrv/usbdrv.h"

#endif /* COMMON_H_ */