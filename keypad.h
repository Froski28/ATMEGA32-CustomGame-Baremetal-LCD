/*
 * keypad.h
 *
 * Created: 5/16/2025 2:01:08 PM
 *  Author: jujuf
 */ 


#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "avr.h"

typedef enum {KEYPAD_NONE = 0, KEYPAD_ONE, KEYPAD_TWO, KEYPAD_THREE, KEYPAD_A,
			  KEYPAD_FOUR, KEYPAD_FIVE, KEYPAD_SIX, KEYPAD_B,
			  KEYPAD_SEVEN, KEYPAD_EIGHT, KEYPAD_NINE, KEYPAD_C,
			  KEYPAD_STAR, KEYPAD_ZERO, KEYPAD_POUND, KEYPAD_D} KeyPad;


int get_key_press();


#endif /* KEYPAD_H_ */