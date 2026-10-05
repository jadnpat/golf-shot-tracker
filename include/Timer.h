/*******************************************************************************
 * @file        Timer.h
 * @brief       Header file for the timer functionality used in the application.
 *
 * @details     This header file contains the function prototypes and definitions 
 *              necessary for interfacing with the timer functionality used in the application.
 * @author      Jared Bawden
 * @date        2026-10-04
 * @version     1.0.0
 *
 ******************************************************************************/

#ifndef TIMER_H
#define TIMER_H

/* Includes */
#include <avr/io.h>
#include <stdint.h>

/* Public API */
void Timer_Init(void);

/* Shared timer state */
extern volatile uint32_t ms_ticks;

#endif // TIMER_H