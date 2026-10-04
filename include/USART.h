/*******************************************************************************
 * @file        USART.h
 * @brief       Header file for the USART0 interface in the ATMega328P.
 *
 * @details     This header file contains the function prototypes and definitions 
 *              necessary for interfacing with the USART0 in the ATMega328P Microcontroller.  
 *              It provides functions to initialise the USART and receive data.
 * @author      Jared Bawden
 * @date        2026-10-03
 * @version     1.0.0
 *
 ******************************************************************************/

#ifndef USART_H
#define USART_H

#include <stdint.h>

/* Function prototypes*/
void USART_Init(void);
uint8_t USART_ReceiveByte(uint8_t *data);
uint8_t USART_GetRxBufferOccupancy(void);

#endif // USART_H