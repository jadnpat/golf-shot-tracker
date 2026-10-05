/*******************************************************************************
 * @file        Buttons.h
 * @brief       Header file for the push buttons used in the application.
 *
 * @details     This header file contains the function prototypes and definitions 
 *              necessary for interfacing with the push buttons used in the application.
 * @author      Jared Bawden
 * @date        2026-10-04
 * @version     1.0.0
 *
 ******************************************************************************/

/****************************************************************************
 
	Here is the pinout for the Buttons and the ATMega328P Microcontroller.  
 
                 -----------                   ----------
                | ATmega328p|                 | Buttons  |
                |           |                 |          |
                |           |                 |          |
                |        PD2|<----------------|BTN_SHOT  |
                |        PD3|<----------------|BTN_HOLE  |
                 -----------                   ----------
 
  **************************************************************************/

#ifndef BUTTONS_H
#define BUTTONS_H

/* Includes */
#include <avr/io.h>
#include <stdint.h>

/* Macros & Constants */
#define BTN_SHOT_PIN    PD2
#define BTN_HOLE_PIN    PD3

/* Public API */
void Buttons_Init(void);

/* Shared interrupt flags */
extern volatile uint8_t shotBtnFlag;
extern volatile uint8_t holeBtnFlag;

#endif // BUTTONS_H