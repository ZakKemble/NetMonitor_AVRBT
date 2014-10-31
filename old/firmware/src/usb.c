/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

#include "usb.h"
#include "usbdrv/usbdrv.h"

USB_PUBLIC uchar usbFunctionSetup(uchar data[8])
{
	usbRequest_t *rq = (void*)data;
	switch(rq->bRequest)
	{
		default:
			break;
	}

    return 0;
}
