/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#include "usb.h"
#include "usbdrv/usbdrv.h"
#include "display.h"
#include "devices/lcd.h"
#include "drivers/spi.h"
#include "btNetMon.h"

#define USB_REQ_IMAGE		1
#define USB_REQ_DRAW		2
#define USB_REQ_BRIGHTNESS	3
#define USB_REQ_BLANK		4
#define USB_REQ_RESET		5

static usbMsgLen_t dataReceived;
static usbMsgLen_t dataLength;

static void pixel(usbWord_t, usbWord_t);

USB_PUBLIC usbMsgLen_t usbFunctionSetup(uchar data[8])
{
	usbRequest_t* rq = (usbRequest_t*)data;
	switch(rq->bRequest)
	{
		case USB_REQ_IMAGE:
			dataReceived = 0;
			dataLength = rq->wLength.word;
			return USB_NO_MSG;
		case USB_REQ_DRAW:
			pixel(rq->wValue, rq->wIndex);
			break;
		case USB_REQ_BRIGHTNESS:
			lcd_setBrightness(rq->wValue.bytes[0]);
			break;
		case USB_REQ_BLANK:
			display_requestBlank();
			break;
		case USB_REQ_RESET:
			reset();
			break;
		default:
			break;
	}

    return 0;
}

USB_PUBLIC uchar usbFunctionWrite(uchar* data, uchar len)
{
	LCD_SELECT(MODE_DATA)
	{
		for(uchar i=0;dataReceived<dataLength && i<len;i++, dataReceived++)
			spi_transfer_nr(data[i]);
	}

	return (dataReceived == dataLength);
}

// Draw a pixel
static void pixel(usbWord_t location, usbWord_t colour)
{
	lcd_setAddrWindow(location.bytes[0], location.bytes[1], location.bytes[0] + 1, location.bytes[1] + 1);
	LCD_SELECT(MODE_DATA)
	{
		spi_transfer_nr(colour.bytes[1]);
		spi_transfer_nr(colour.bytes[0]);
	}
}
