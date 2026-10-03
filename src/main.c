#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <LCD.h>

int main(void) {

    LCD_Init(); // Initialize the LCD
	LCD_WriteString("Hole 01, Shot 01", TOP); // Write a string to the top row

    while (1) {
        LCD_WriteString("            285m", BOTTOM);
        _delay_ms(1000); // Delay for 1 second
        LCD_WriteString("              0m", BOTTOM);
        _delay_ms(1000); // Delay for 1 second
    }

    return 0;
}