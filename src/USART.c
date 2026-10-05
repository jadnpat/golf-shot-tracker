/*******************************************************************************
 * @file        USART.c
 * @brief       Configures USART0 and buffers received GPS bytes.
 *
 * @details     Initializes USART0 and provides access to bytes queued by
 *              the receive interrupt.
 * @author      Jared Bawden
 * @date        2026-10-03
 * @version     1.0.0
 ******************************************************************************/

/* Includes */
#include "USART.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Macros & Constants */
#define BAUD 9600
#define MYUBRR (((F_CPU / (BAUD * 16UL))) - 1)
#define RX_BUFFER_SIZE 64

/* Global Variables */
static volatile uint8_t rxBuffer[RX_BUFFER_SIZE];
static volatile uint8_t rxHead = 0;
static volatile uint8_t rxTail = 0;

/* Public Functions */
/**
 * @brief Initialize USART0 for 9600-baud, 8-N-1 reception.
 * @return None.
 */
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

/**
 * @brief Remove the next byte from the USART receive queue, if available.
 * @param data Destination for the received byte.
 * @return 1 if a byte was returned; otherwise 0 when the queue is empty.
 */
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

/**
 * @brief Get the number of bytes currently queued for reception.
 * @return Current receive-queue occupancy.
 */
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

/* Interrupts */
ISR(USART_RX_vect) {
    uint8_t data = UDR0;
    uint8_t nextHead = (rxHead + 1) % RX_BUFFER_SIZE;

    if (nextHead != rxTail)
    {
        rxBuffer[rxHead] = data;
        rxHead = nextHead;
    }
}