/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>
#include <util/twi.h>
#include "common.h"
#include "drivers/i2c.h"

#define SDA			C4
#define SDL			C5
#define TWI_FREQ	200000UL

void i2c_init()
{
	TWBR = (((F_CPU / TWI_FREQ) - 16) / 2);

	// Set I2C to known state (just in case)
	i2c_resetState();

	// Disable I2C
	power_twi_disable();
}

// Reset to known state
void i2c_resetState()
{
	bit_clr(TWCR, TWEN);
	pinMode(SDA, INPUT);
	pinPullup(SDA, PULLUP_ENABLE);
	delay(1);

	// Toggle SDL until SDA goes HIGH or times out
	byte count = 64; // 64 * 1ms = 64ms timeout
	while(!pinRead(SDA) && count--)
	{
		delay(1);
		pinWrite(SDL, TOGGLE);
	}

	// Back to normal
	pinMode(SDA, OUTPUT);
	pinMode(SDL, OUTPUT);
	pinWrite(SDA, HIGH);
	pinWrite(SDL, HIGH);
}

void i2c_start()
{
	// Enable I2C
	power_twi_enable();

	// START
	TWCR = _BV(TWINT)|_BV(TWSTA)|_BV(TWEN);

	// Wait for START
	loop_until_bit_is_set(TWCR, TWINT);
}

void i2c_stop()
{
	// STOP
	TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWSTO);

	// Wait for STOP
	// WARNING: A bad I2C line can cause this loop to hang
	//loop_until_bit_is_set(TWCR, TWSTO);
	byte counter = 255; // 255 * 5us = 1.275ms timeout
	while(bit_is_clear(TWCR, TWSTO) && counter--)
		delay_us(5);

	// Disable I2C
	bit_clr(TWCR, TWEN);

	power_twi_disable();
}

bool i2c_stop_error()
{
	i2c_stop();
	return false;
}

bool i2c_write(byte data)
{
	// Set data
	TWDR = data;

	// Begin sending
	TWCR = _BV(TWINT)|_BV(TWEN);

	// Wait for finish
	loop_until_bit_is_set(TWCR, TWINT);

	// Check status
	byte status = TW_STATUS;
	return (status == TW_MT_SLA_ACK || status == TW_MT_DATA_ACK || status == TW_MR_SLA_ACK);
}

bool i2c_read(byte* data, bool ack)
{
	// Expects more data (ACK) or not (NACK)
	ack ? (bit_set(TWCR, TWEA)) : (bit_clr(TWCR, TWEA));

	// Begin receiving
	bit_set(TWCR, TWINT);

	// Wait for finish
	loop_until_bit_is_set(TWCR, TWINT);

	// Check status
	byte status = TW_STATUS;
	if(status != TW_MR_DATA_ACK && status != TW_MR_DATA_NACK)
		return false;

	// Read data
	*data = TWDR;
	return true;
}
