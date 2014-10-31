/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#include "common.h"
#include "devices/lm73_defs.h"

//#define TEMP_OFFSET	8

static bool read(byte, byte*, byte);
//static bool write(byte, byte);
/*
void lm73_init()
{
	
}
*/
byte lm73_getTemp()
{
	byte data[2];
	read(LM73ADDR_TEMP, data, 2);

	// Super basic method of converting the temperature,
	// but only supports 0C - 127C in 1C increments.
	byte temp = data[0] << 1;
	if(data[1] & 0b10000000)
		temp++;
//	temp -= TEMP_OFFSET;

//	printf("%hhu\n", temp);

	return temp;
}

// I2C read
static bool read(byte address, byte* data, byte count)
{
	// Start
	i2c_start();

	// Write to set register address
	if(!i2c_write(LM73I2C_W)) // Slave address + write
		return i2c_stop_error();	
	else if(!i2c_write(address)) // Register address
		return i2c_stop_error();

	// Repeat start
	i2c_start();

	// Set register address
	if(!i2c_write(LM73I2C_R)) // Slave address + read
		return i2c_stop_error();

	// Read data
	for(byte i=0;i<count;i++)
	{
		if(!i2c_read(&data[i], (i < (count - 1)) ? I2C_ACK : I2C_NACK))
			return i2c_stop_error();
	}

	// Stop
	i2c_stop();

	return true;
}
/*
// I2C write
static bool write(byte address, byte data)
{
	// Start
	i2c_start();

	// Write data at register address
	if(!i2c_write(LM73I2C_W)) // Slave address + write
		return i2c_stop_error();
	else if(!i2c_write(address)) // Register address
		return i2c_stop_error();
	else if(!i2c_write(data)) // Write data
		return i2c_stop_error();

	// Stop
	i2c_stop();
	
	return true;
}
*/