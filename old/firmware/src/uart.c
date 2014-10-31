/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

#include "common.h"
#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define BAUD CFG_BAUD
#include <util/setbaud.h>

#define UART_RXBUFFSIZE 71

volatile static byte rxBuffer[UART_RXBUFFSIZE];
volatile static byte *volatile rxHead;
volatile static byte *volatile rxTail;

void uart_init()
{
	PORTD &= ~(1<<PORTD0);
	
	// UART settings
	UBRR0 = UBRR_VALUE;
#if USE_2X
	UCSR0A = (1<<U2X0);
#endif
	UCSR0B = (1<<RXEN0)|(1<<RXCIE0);
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);

	rxHead = rxBuffer;
	rxTail = rxBuffer;
}

// Get next value
bool uart_get_nb(byte* b)
{
	if(rxHead == rxTail)
		return false;

	*b = *rxTail;
	if (++rxTail == (rxBuffer + UART_RXBUFFSIZE))
		rxTail = rxBuffer;

	return true;
}

ISR(USART_RX_vect)
{
	int diff = rxHead - rxTail;
	if(diff < 0)
		diff += UART_RXBUFFSIZE;

	if(diff < UART_RXBUFFSIZE - 1)
	{
		*rxHead = UDR0;
		if (++rxHead == (rxBuffer + UART_RXBUFFSIZE))
			rxHead = rxBuffer;
	}
	else // Buffer full
		UDR0;
}
