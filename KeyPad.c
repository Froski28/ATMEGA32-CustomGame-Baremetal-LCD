/*
 * KeyPad.c
 *
 * Created: 5/16/2025 2:00:16 PM
 *  Author: jujuf
 */

#include "keypad.h"

int is_pressed(int r, int c)
{
	// Fix C to be where column pins start - at pins 5-8 - AT PC3-PC0
	c = PC3 - c;
	// Fix R since orientation of pins, row starts at PC7
	r = 7-r;
	// Set all DDRC & PortC to 0
	DDRC = 0;
	PORTC = 0;
	// Set row pin to be ground
	DDRC |= (1 << r);
	// Set column pin to weak'1'
	PORTC |= (1 << c);
	// Check output & return
	if (!(PINC & (1 << c)))
	return 1;
	return 0;
}

int pressed_and_stable(int r, int c, int ms)
{
	int a = is_pressed(r, c);
	avr_wait(ms);
	int b = is_pressed(r, c);
	
	return (a && b) ? 1: 0;
}

/** Does not get combinations, just first key press sequentially from 1,2,...,16 */
int get_key_press()
{
	for (int r=0; r < 4; ++r){
		for (int c=0; c < 4; ++c){
			if (pressed_and_stable(r, c, 1))
			{
				return ((r*4)+(c+1));
			}
		}
	}
	return 0;
}
