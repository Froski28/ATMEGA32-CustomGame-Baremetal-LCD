/*
 * avr.c
 *
 * Created: 4/15/2025 10:23:40 AM
 *  Author: jujuf
 */ 

#include "avr.h"

void
avr_init(void)
{
	WDTCR = 15;
}

void
avr_wait(unsigned short msec)
{
	TCCR0 = 3;
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}

void avr_wait_micro(int microsec)
{
	  TCCR1A = 0; // Disable PWM
	  TCCR1B = (1 << CS11);  // Prescale by 8 since clock freq is 8MHz 8000000 / 
	  TCNT1 = 0; // Set timer to zero
	  while (TCNT1 < microsec); // Count timer
	  TCCR1B = 0;  // Stop timer
}