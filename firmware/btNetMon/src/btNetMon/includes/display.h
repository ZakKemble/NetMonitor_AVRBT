/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#define DISPLAY_MODE1		0
#define DISPLAY_MODE2		1
#define DISPLAY_MODE_BLANK	255

void display_setMode(byte);
void display_requestBlank(void);
void display_update(s_monitorData*);

#endif /* DISPLAY_H_ */