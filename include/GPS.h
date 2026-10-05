/*******************************************************************************
 * @file        GPS.h
 * @brief       Header file for the NEO-7M-based GPS module used with ATMega328P.
 *
 * @details     This header file contains the function prototypes and definitions 
 *              necessary for interfacing with the NEO-7M GPS module using an ATMega328P Microcontroller.  
 *              It provides functions to read data, and parse NMEA sentences.
 * @author      Jared Bawden
 * @date        2026-10-03
 * @version     1.0.0
 *
 ******************************************************************************/

/****************************************************************************
 
	Here is the pinout for the GPS module and the ATMega328P Microcontroller.  
    The GPS module is a .
 
                 -----------                   ----------
                | ATmega328p|                 |   GPS    |
                |           |                 |          |
                |        PD7|<----------------|PPS       |
                |        PD6|---------------->|RXD       |
                |        PD5|<----------------|TXD       | // Note, not used in this project
                 -----------                   ----------
 
  **************************************************************************/

#ifndef GPS_H
#define GPS_H

/* Includes */
#include <avr/io.h>
#include "USART.h"

/* Macros & Constants */
#define POINT_INVALID 0xFFFF

/* Type definitions */
typedef struct
{
    uint16_t lat;
    uint16_t lon;
} Point_t;

/* Public API */
void GPS_Main(void);
Point_t GPS_GetCurrentPoint(void);
uint8_t GPS_HasValidPoint(void);
uint8_t GPS_GetLonScaleFactor(void);

#endif // GPS_H