/*******************************************************************************
 * @file        LCD.h
 * @brief       Header file for the HD44780 LCD module used with ATMega328P.
 *
 * @details     This header file contains the function prototypes and definitions 
 *              necessary for interfacing with the HD44780 LCD module in 4-bit mode
 *              using an ATMega328P Microcontreoller.  
 *              It provides functions to initialize the LCD, write characters and 
 *              instructions, and manage the display.
 *              .
 * @author      Jared Bawden
 * @date        2026-10-03
 * @version     1.0.0
 *
 ******************************************************************************/

/****************************************************************************
 
	Here is the pinout for the LCD module and the ATMega328P Microcontroller.  
    The LCD module is a standard HD44780 16x2 character display.
 
                 -----------                   ----------
                | ATmega328p|                 |   LCD    |
                |           |                 |          |
                |        PD7|---------------->|D7        |
                |        PD6|---------------->|D6        |
                |        PD5|---------------->|D5        |
                |        PD4|---------------->|D4        |
                |           |                 |D3        |
                |           |                 |D2        |
                |           |                 |D1        |
                |           |                 |D0        |
                |        PB2|---------------->|E         |
                |        PB1|---------------->|R/W       |
                |        PB0|---------------->|RS        |
                 -----------                   ----------
 
  **************************************************************************/

#ifndef LCD_H
#define LCD_H

/* LCD pin definitions */
#define DATA_PORT PORTD
#define DATA_DDR DDRD

#define CONTROL_PORT PORTB
#define CONTROL_DDR DDRB

#define RS_PIN PB0
#define RW_PIN PB1
#define E_PIN PB2

/* LCD command definitions */
typedef enum
{
    CLEARDISPLAY = 0x01,
    RETURNHOME = 0x02,
    ENTRYMODESET = 0x04,
    DISPLAYCONTROL = 0x08,
    CURSORSHIFT = 0x10,
    FUNCTIONSET = 0x20,
    SETCGRAMADDR = 0x40,
    SETDDRAMADDR = 0x80
} Command_t;

typedef enum
{
    ENTRYRIGHT = 0x00,
    ENTRYLEFT = 0x02
} EntryMode_t;

typedef enum
{
    ENTRYSHIFTDECREMENT = 0x00,
    ENTRYSHIFTINCREMENT = 0x01
} EntryShiftMode_t;

typedef enum
{
    DISPLAYOFF = 0x00,
    DISPLAYON = 0x04
} DisplayControl_t;

typedef enum
{
    CURSOROFF = 0x00,
    CURSORON = 0x02
} CursorControl_t;

typedef enum
{
    BLINKOFF = 0x00,
    BLINKON = 0x01
} BlinkControl_t;

typedef enum
{
    CURSORMOVE = 0x00,
    DISPLAYMOVE = 0x08
} ShiftMode_t;

typedef enum
{
    MOVELEFT = 0x00,
    MOVERIGHT = 0x04
} ShiftDirection_t;

typedef enum
{
    BITMODE4 = 0x00,
    BITMODE8 = 0x10
} BitMode_t;

typedef enum
{
    ONELINE = 0x00,
    TWOLINE = 0x08
} LineMode_t;

typedef enum
{
    DOTS5x8 = 0x00,
    DOTS5x10 = 0x04
} FontMode_t;

typedef enum
{
    CMD = 0,
    DATA = 1
} SendMode_t;

typedef enum
{
    TOP = 0,
    BOTTOM = 1
} Row_t;

/* Function prototypes */
void LCD_Init(void);
void LCD_Clear(void);
void LCD_WriteString(char *str, Row_t row);

#endif // LCD_H