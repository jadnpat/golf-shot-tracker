/*******************************************************************************
 * @file        main.c
 * @brief       Initializes peripherals and runs the GPS and shot-tracker loop.
 *
 * @details     Initializes the application modules and repeatedly processes
 *              GPS input and shot-tracker state.
 * @author      Jared Bawden
 * @date        2026-10-03
 * @version     1.0.0
 ******************************************************************************/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/* Includes */
#include <avr/io.h>
#include <util/delay.h>
#include "LCD.h"
#include "USART.h"
#include "GPS.h"
#include "Timer.h"
#include "Buttons.h"
#include "ShotTracker.h"
#include <stdio.h>

/* Public Functions */
/**
 * @brief Initialize the firmware and run its main processing loop.
 * @return Zero if the main loop exits.
 */
int main(void) {
    USART_Init();
    LCD_Init();
    Timer_Init();
    Buttons_Init();
    ShotTracker_Init();

    while (1) {
        GPS_Main();
        ShotTracker_Main();
    }

    return 0;
}