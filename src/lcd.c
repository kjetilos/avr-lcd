#include "lcd.h"
#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>

#define ENABLE				PIND0
#define READ_WRITE			PIND1
#define REGISTER_SELECT		PIND2

static void check_busy(void);
static void send_command(unsigned char cmd);
static void send_char(unsigned char c);
static void toggle(void);
static void next_line(void);

static unsigned char pos=0;

void lcd_show(const char * s)
{
	while (*s)
	{
		char c = *s;
		if (c == 0xf8)
			send_char(0);
		else if (c == 0xe6)
			send_char(1);
		else
			send_char(c);
		s++;
		_delay_ms(50);
		pos++;
		if (pos == 0x10)
			next_line();
	}
}

void lcd_kill(void)
{
	PORTB = 0x0; // Turn off all output
	PORTD &= ~((1 << ENABLE) | (1 << READ_WRITE) | (1 << REGISTER_SELECT));
}

static void create_char(void)
{
	send_command(0x40 | 0x0); // Set CGRAM 0x0
	_delay_us(37);
	// 0 -> ø
	send_char(0b00000);
	send_char(0b00010);
	send_char(0b01110);
	send_char(0b10101);
	send_char(0b10101);
	send_char(0b01110);
	send_char(0b01000);
	send_char(0b00000);
	
	// 1 -> æ
	send_char(0b00000);
	send_char(0b00000);
	send_char(0b11010);
	send_char(0b00101);
	send_char(0b01111);
	send_char(0b10100);
	send_char(0b01111);
	send_char(0b00000);
	
	send_command(0x80 | 0x0); // Set DDRAM 0x0
	_delay_us(37);
}

void lcd_init(void)
{
	DDRD |= 1 << ENABLE | 1 << READ_WRITE | 1 << REGISTER_SELECT;
	/* Display Initialization according to ST7066U Datasheet */
	_delay_ms(40);
	send_command(0x38);
	_delay_us(37);
	send_command(0x38);
	_delay_us(37);
	send_command(0b00001100);
	_delay_us(37);
	send_command(0x01);
	_delay_ms(2);
	send_command(0x6);
	_delay_us(37);
	create_char();
	pos=0;
}

static void check_busy(void)
{
	DDRB = 0;
	PORTD |= 1 << READ_WRITE;
	PORTD &= ~(1 << REGISTER_SELECT);
	while (PORTB >= 0x80)
	{
		toggle();
	}
	DDRB = 0xff;
}

static void toggle(void)
{
	PORTD |= 1 << ENABLE;
	asm volatile("nop");
	asm volatile("nop");
	PORTD &= ~ (1 << ENABLE);
}

static void send_command(unsigned char cmd)
{
	check_busy();
	PORTB = cmd;
	PORTD &= ~ ((1<<READ_WRITE) | (1<<REGISTER_SELECT));
	toggle();
	PORTB = 0;
}

static void send_char(unsigned char c)
{
	check_busy();
	PORTB = c;
	PORTD &= ~ (1 << READ_WRITE);
	PORTD |= 1 << REGISTER_SELECT;
	toggle();
	PORTB = 0;
}

static void next_line(void)
{
	send_command(0xc0); // Set DDRAM Address to 0xx40
	_delay_us(37);
}