/*******************************************************************************
 * @file        ShotTracker.h
 * @brief       Header file for the Shot Tracker application code.
 *
 * @details     This header file contains the function prototypes and definitions 
 *              necessary for the handling of the peripheral data to run the golf
 *              shot tracker application. Keeps track of shots and manages the 
 *              information printed on the LCD based on data from the GPS and push
 *              buttons.
 * @author      Jared Bawden
 * @date        2026-10-05
 * @version     1.0.0
 *
 ******************************************************************************/
#ifndef SHOTTRACKER_H
#define SHOTTRACKER_H

/* Public API */
void ShotTracker_Init(void);
void ShotTracker_Main(void);

#endif // SHOTTRACKER_H