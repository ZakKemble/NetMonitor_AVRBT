/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

#ifndef NETSPEED_H_
#define NETSPEED_H_

#define BTN_PPORT	PINPORT(CFG_BTN_PORT)
#define BTN_PIN		PINBIT(CFG_BTN_PORT, CFG_BTN_BIT)

#define BTN_PRESSED()	(!(BTN_PPORT & (1<<BTN_PIN)))

#endif /* NETSPEED_H_ */