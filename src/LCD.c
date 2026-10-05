/*******************************************************************************
 * @file        LCD.c
 * @brief       Drives the HD44780 16x2 display in 4-bit mode.
 *
 * @details     Initializes the display and provides functions for clearing
 *              it and writing fixed-width text rows.
 * @author      Jared Bawden
 * @date        2026-10-03
 * @version     1.0.0
 ******************************************************************************/

/* Includes */
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "LCD.h"

/* Macros & Constants */
#ifndef F_CPU
#define F_CPU 16000000UL // Define CPU frequency for delay functions
#endif

/* Private Prototypes */
static void send(uint8_t data, SendMode_t mode);
static void send4Bits(uint8_t data);
static void pulseEnable(void);

/* Public Functions */
/**
 * @brief Initialize the LCD in 4-bit, two-row mode.
 * @return None.
 */
void LCD_Init(void)
{
    /* Set data pins as output */
    DATA_DDR |= (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);

    /* Set control pins as output*/
    CONTROL_DDR |= (1 << RS_PIN) | (1 << RW_PIN) | (1 << E_PIN);

    /* Pull R/W low (write mode) */
    CONTROL_PORT &= ~(1 << RW_PIN);

    /* Follow initialization sequence described in the HD44780 datasheet */
    _delay_ms(50); // Wait for more than 40 ms after VCC rises to 2.7V
    send4Bits(0x03);
    _delay_ms(5);  // Wait for more than 4.1 ms
    send4Bits(0x03);
    _delay_us(150); // Wait for more than 100 us
    send4Bits(0x03);
    send4Bits(0x02); // Set to 4-bit mode

    /* Initialize the LCD in 4-bit mode with 2 lines and 5x8 font */
    send(FUNCTIONSET | BITMODE4 | TWOLINE | DOTS5x8, CMD);

    /* Turn on the display, cursor off, blink off */
    send(DISPLAYCONTROL | DISPLAYON | CURSOROFF | BLINKOFF, CMD);

    /* Clear the display */
    send(CLEARDISPLAY, CMD);
    _delay_ms(2);

    /* Set entry mode and direction */
    send(ENTRYMODESET | ENTRYLEFT | ENTRYSHIFTDECREMENT, CMD);
}

/**
 * @brief Clear the LCD and return its cursor to the home position.
 * @return None.
 */
void LCD_Clear(void)
{
    send(CLEARDISPLAY, CMD); // Clear display command
    _delay_ms(2);
}

/**
 * @brief Write a padded or truncated 16-character string to an LCD row.
 * @param str Null-terminated text to display.
 * @param row LCD row to write, either TOP or BOTTOM.
 * @return None.
 */
void LCD_WriteString(char *str, Row_t row)
{
    if (row == TOP)
    {
        send(SETDDRAMADDR | 0x00, CMD); // Set cursor to the beginning of the first line
    }
    else if (row == BOTTOM)
    {
        send(SETDDRAMADDR | 0x40, CMD); // Set cursor to the beginning of the second line
    }

    for (uint8_t i = 0; i < 16; i++)
    {
        send(*str ? *str++ : ' ', DATA);
    }
}

/* Private Functions */
static void send(uint8_t data, SendMode_t mode)
{

    /* Ensure that R/W is low (write mode) */
    CONTROL_PORT &= ~(1 << RW_PIN);

    if (mode == CMD)
    {
        // Set RS low for command
        CONTROL_PORT &= ~(1 << RS_PIN);
    }
    else
    {
        // Set RS high for data
        CONTROL_PORT |= (1 << RS_PIN);
    }

    // Send the upper nibble
    send4Bits(data >> 4);

    // Send the lower nibble
    send4Bits(data & 0x0F);
}

static void send4Bits(uint8_t data)
{
    PORTD = (PORTD & 0x0F) | ((data << 4) & 0xF0);

    pulseEnable();
}

static void pulseEnable(void)
{
    CONTROL_PORT &= ~(1 << E_PIN);
    _delay_us(1);

    // Set E high
    CONTROL_PORT |= (1 << E_PIN);
    _delay_us(1); // Enable pulse width must be at least 450 ns

    // Set E low
    CONTROL_PORT &= ~(1 << E_PIN);
    _delay_us(100); // Commands need >37 us to settle
}