/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

void bluetooth_init(void);
bool bluetooth_led(void);
void bluetooth_update(s_monitorData*);

#endif /* BLUETOOTH_H_ */