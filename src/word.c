#include "word.h"
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#define NUM_STRINGS 5
uint8_t eeprom_select EEMEM = 0;
static const char words[] =
	"Du er verdifull\0"
	"Du er vakker\0"
	"Jeg er glad i deg\0"
	"Elsk din neste som deg selv\0"
	"St\xf8rst av alt er kj\xe6rligheten\0";

static const char * find_word(uint8_t n)
{
	const char * start = words;
	const char * end = words+sizeof(words);
	
	if (n == 0)
		return start;
	
	while (start < end)
	{
		char c = *start++;
		if (c == '\0')
		{
			n--;
		}
		if (n == 0)
		{
			return start;
		}
	}
	return NULL;
}

const char * next_word(void)
{
	// Read and update counter
	uint8_t select = eeprom_read_byte(&eeprom_select);
	eeprom_write_byte(&eeprom_select, select+1);
		
	const char * word = find_word(select % NUM_STRINGS);
	if (!word)
	{
		word = words;
	}
	return word;
}
