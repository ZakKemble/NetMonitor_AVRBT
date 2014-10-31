/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#ifndef LCD_H_
#define LCD_H_

#define LCD_WIDTH 160
#define LCD_HEIGHT 128

#define BRIGHTNESS_HIGH 255
#define BRIGHTNESS_MID 70
#define BRIGHTNESS_LOW 10

#define ROTATION_NORMAL 0b01100000
#define ROTATION_BARGRAPH 0b00000000

#define CS	B2
#define RS	B1

#define spiDeselect()	(pinWrite(CS, HIGH))
#define spiSelect()		(pinWrite(CS, LOW))
#define dataMode()		(pinWrite(RS, HIGH))
#define cmdMode()		(pinWrite(RS, LOW))

static inline bool lcd_select(void)
{
	spiSelect();
	return true;
}

static inline bool lcd_deselect(void)
{
	spiDeselect();
	return false;
}

#define MODE_CMD	(cmdMode())
#define MODE_DATA	(dataMode())
#define LCD_SELECT(mode) mode; \
	for (bool cs = lcd_select(); cs; cs = lcd_deselect())

void lcd_init(void);
void lcd_writeCommand(byte);
void lcd_writeData(byte);
void lcd_setRotation(byte);
void lcd_setAddrWindow(byte, byte, byte, byte);
void lcd_blank(void);
void lcd_setBrightness(byte);

#endif /* LCD_H_ */
