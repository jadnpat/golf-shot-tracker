/*******************************************************************************
 * @file        Timer.c
 * @brief       Configures Timer0 and maintains the millisecond tick counter.
 *
 * @details     Sets Timer0 to generate a compare-match interrupt every
 *              millisecond and increments the shared tick counter.
 * @author      Jared Bawden
 * @date        2026-10-04
 * @version     1.0.0
 ******************************************************************************/

/* Includes */
#include "Timer.h"
#include <avr/interrupt.h>

/* Global Variables */
volatile uint32_t ms_ticks = 0;

/* Public Functions */
/**
 * @brief Configure Timer0 to generate a one-millisecond tick interrupt.
 * @return None.
 */
void Timer_Init(void)
{
    /* Set CTC mode (Clear Timer on Compare Match) */
    TCCR0A = (1 << WGM01);
    /* Set top count value for 1ms interval: (16,000,000Hz / 64 prescaler / 1000Hz) - 1 = 249 */
    OCR0A = 249;
    /* Enable Compare Match A interrupt */
    TIMSK0 = (1 << OCIE0A);
    /* Set prescaler to 64 and start timer */
    TCCR0B = (1 << CS01) | (1 << CS00);
}

/* Interrupts */
ISR(TIMER0_COMPA_vect) {
    ms_ticks++;
}