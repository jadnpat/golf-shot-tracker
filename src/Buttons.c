/*******************************************************************************
 * @file        Buttons.c
 * @brief       Handles debounced shot and hole button interrupts.
 *
 * @details     Configures the button inputs and records debounced presses
 *              from the external interrupt handlers.
 * @author      Jared Bawden
 * @date        2026-10-04
 * @version     1.0.0
 ******************************************************************************/

/* Includes */
#include "Buttons.h"
#include "Timer.h"
#include <avr/interrupt.h>

/* Macros & Constants */
#define DEBOUNCE_TICKS 75

/* Global Variables */
volatile uint8_t shotBtnFlag = 0;
volatile uint8_t holeBtnFlag = 0;
static volatile uint32_t lastShotBtnTicks = 0;
static volatile uint32_t lastHoleBtnTicks = 0;

/* Public Functions */
/**
 * @brief Configure the shot and hole buttons and enable their interrupts.
 * @return None.
 */
void Buttons_Init(void)
{
    shotBtnFlag = 0;
    holeBtnFlag = 0;

    /* ISC01=1, ISC00=0 -> falling edge for INT0 */
    /* ISC11=1, ISC10=0 -> falling edge for INT1 */
    EICRA |= (1 << ISC01) | (1 << ISC11);
    EICRA &= ~((1 << ISC00) | (1 << ISC10));

    /* Enable External Interrupts INT0 and INT1 in EIMSK */
    EIMSK |= (1 << INT0) | (1 << INT1);

    /* Ensure button pins are configured as inputs*/
    DDRD &= ~((1 << DDD2) | (1 << DDD3));

    /* Enable the internal pull-up resistors */
    PORTD |= (1 << BTN_SHOT_PIN) | (1 << BTN_HOLE_PIN);

    /* Enable global interrupts */
    sei(); 
}

/* Interrupts */
ISR(INT0_vect)
{
    uint32_t currentTicks = ms_ticks;

    if((currentTicks - lastShotBtnTicks) > DEBOUNCE_TICKS)
    {
        shotBtnFlag = 1;

        lastShotBtnTicks = currentTicks;
    }
}

ISR(INT1_vect)
{
    uint32_t currentTicks = ms_ticks;

    if((currentTicks - lastHoleBtnTicks) > DEBOUNCE_TICKS)
    {
        holeBtnFlag = 1;

        lastHoleBtnTicks = currentTicks;
    }
}