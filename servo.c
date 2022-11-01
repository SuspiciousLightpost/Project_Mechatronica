/*
 * servo.c - XvR 2020
 *
 * Use 16-bit timer 1. Using interrupts in order to be
 * able to use the pins on the multi-function shield
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "servo.h"

#define TIME_VALUE	(40000)
#define RESET_VALUE	(65636ul-TIME_VALUE)
#define STOP_VALUE	(TIME_VALUE*0.075)
#define RANGE		(TIME_VALUE*0.025)

ISR(TIMER1_OVF_vect)
{
	TCNT1 = RESET_VALUE;

	PORT_1 |= (1<<PIN_1);
	PORT_2 |= (1<<PIN_2);
}

ISR(TIMER1_COMPA_vect)
{
	PORT_1 &= ~(1<<PIN_1);
}

ISR(TIMER1_COMPB_vect)
{
	PORT_2 &= ~(1<<PIN_2);
}

void init_servo(void)
{
	// Config pins as output
	DDR_1 |= (1<<PIN_1);
	DDR_2 |= (1<<PIN_2);

	// Use mode 0, clkdiv = 8
	TCCR1A = 0;
	TCCR1B = (0<<CS12) | (1<<CS11) | (0<<CS10);
	// Interrupts on OCA, OCB and OVF
	TIMSK1 = (1<<OCIE1B) | (1<<OCIE1A) | (1<<TOIE1);

	TCNT1 = RESET_VALUE;

	servo1_set_percentage(0);
	servo2_set_percentage(0);

	sei();
}

void servo1_set_percentage(signed char percentage)
{
	if (percentage >= -100 && percentage <= 100)
	{
		OCR1A = RESET_VALUE+STOP_VALUE+(RANGE/100*percentage);
	}
}

void servo2_set_percentage(signed char percentage)
{
	if (percentage >= -100 && percentage <= 100)
	{
		OCR1B = RESET_VALUE+STOP_VALUE+(RANGE/100*percentage);
	}
}
