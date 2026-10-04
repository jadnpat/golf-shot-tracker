#include "USART.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define BAUD 9600
#define MYUBRR (((F_CPU / (BAUD * 16UL))) - 1)
#define RX_BUFFER_SIZE 64

static volatile uint8_t rxBuffer[RX_BUFFER_SIZE];
static volatile uint8_t rxHead = 0;
static volatile uint8_t rxTail = 0;

/* *****************************************************************
Name:		USART_Init
Inputs:		none
Outputs:	none
Description:Configures and initializes the USART
******************************************************************** */
void USART_Init(void) {
    /* Set baud rate to 9600 */
    UBRR0H = (unsigned char)(MYUBRR >> 8);
    UBRR0L = (unsigned char)MYUBRR;
    
    /* Enable RX and RX Interrupt*/
    UCSR0B = (1 << RXCIE0) | (1 << RXEN0);

    /* 8 data bits, 1 stop bit*/
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    sei();
}

/* *****************************************************************
Name:		USART_ReceiveByte
Inputs:		Pointer to a variable where the received byte will be stored
Outputs:	Returns 1 if a byte was received, 0 if the buffer is empty
Description:Receives a byte from the USART receive buffer
******************************************************************** */
uint8_t USART_ReceiveByte(uint8_t *data)
{
    if (rxTail == rxHead)
    {
        return 0;
    }

    *data = rxBuffer[rxTail];
    rxTail = (rxTail + 1) % RX_BUFFER_SIZE;
    return 1;
}

/* *****************************************************************
Name:		USART_GetRxBufferOccupancy
Inputs:		none
Outputs:	Returns the number of bytes currently in the receive buffer
Description:Returns the number of bytes currently in the receive buffer
******************************************************************** */
uint8_t USART_GetRxBufferOccupancy(void)
{
    uint8_t head = rxHead;
    uint8_t tail = rxTail;

    if (head >= tail)
    {
        return head - tail;
    }

    return RX_BUFFER_SIZE - tail + head;
}

ISR(USART_RX_vect) {
    uint8_t data = UDR0;
    uint8_t nextHead = (rxHead + 1) % RX_BUFFER_SIZE;

    if (nextHead != rxTail)
    {
        rxBuffer[rxHead] = data;
        rxHead = nextHead;
    }
}