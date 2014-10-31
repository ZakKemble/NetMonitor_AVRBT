/* * Project: Bluetooth Net Monitor * Author: Zak Kemble, contact@zakkemble.co.uk * Copyright: (C) 2013 by Zak Kemble * License: GNU GPL v3 (see License.txt) * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/ */

#include "common.h"

#define BAUD CFG_BAUD

#include <util/setbaud.h>

#define UART_ENABLE_TX	0
#define UART_RXBUFFSIZE 22

typedef struct{
	byte data[UART_RXBUFFSIZE];
	byte head;
	byte tail;
}s_ringBuffer;

#if UART_ENABLE_TX
static int put(char, FILE*);
FILE uart_io = FDEV_SETUP_STREAM(put, NULL, _FDEV_SETUP_WRITE);
#endif

static volatile s_ringBuffer rxBuffer;

void uart_init()
{
	// Pullup on RXD
	pinPullup(D0, PU_EN);

	// UART settings
	UBRR0 = UBRR_VALUE;
#if USE_2X
	UCSR0A = _BV(U2X0);
#endif
#if UART_ENABLE_TX
	UCSR0B = _BV(TXEN0)|_BV(RXEN0)|_BV(RXCIE0);
#else
	UCSR0B = _BV(RXEN0)|_BV(RXCIE0);
#endif
	UCSR0C = _BV(UCSZ00)|_BV(UCSZ01);

#if UART_ENABLE_TX
	stdout = &uart_io;
#endif
}

// Get next value
bool uart_get_nb(byte* b)
{// THIS NEEDS TO BE ATOMIC! (disable UART rx ISR then enable again)
	// Empty
	if(rxBuffer.head == rxBuffer.tail)
		return false;

	*b = rxBuffer.data[rxBuffer.tail];
	if (++rxBuffer.tail >= UART_RXBUFFSIZE)
		rxBuffer.tail = 0;

	return true;
}

#if UART_ENABLE_TX
static int put(char c, FILE* stream)
{
	UNUSED(stream); // Get rid of unused variable warning
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	return 0;
}
#endif

ISR(USART_RX_vect)
{
	int8_t diff = rxBuffer.head - rxBuffer.tail;
	if(diff < 0)
		diff += UART_RXBUFFSIZE;

	if(diff < UART_RXBUFFSIZE - 1)
	{
		rxBuffer.data[rxBuffer.head] = UDR0;
		if (++rxBuffer.head >= UART_RXBUFFSIZE)
			rxBuffer.head = 0;
	}
	else // Buffer full
		UDR0;
}
