#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "LCD.h"
#include "USART.h"
#include "GPS.h"
#include <stdio.h>

int main(void) {
    USART_Init(); // Initialize the UART
    LCD_Init(); // Initialize the LCD

    while (1) {
        GPS_Main();
    }

    return 0;
}