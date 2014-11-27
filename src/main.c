#define F_CPU 1000000UL

#include "lcd.h"
#include "word.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

void setup(void)
{
	DDRA |= (1 << PA1) | (1 << PA0);
	PORTA |= (1 << PA1) | (1 << PA0);

	lcd_init();
	lcd_show(next_word());
}

static void shutdown(void)
{
	// Toggle transistors as well
	PORTA &= ~((1 << PA1) | (1 << PA0));
	lcd_kill();
	while (1)
	{
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();		
	}
}

int main(void)
{
	setup();
	_delay_ms(10000);
	shutdown();
}
