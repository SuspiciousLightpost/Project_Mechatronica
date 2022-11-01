#include <avr/io.h>
#include <util/delay.h>

#include "h_bridge.h"
#include "servo.h"

// Buttons
#define stopLaag    PF0 // analog 0
#define stopHoog    PF1 // analog 1
#define knopOpen    PF2 // analog 2
#define knopDicht   PF3 // analog 3

// Lights
#define groenWeg    PB0 // digital 53
#define roodWeg     PB1 // digital 52

#define groenWater  PB2 // digital 51
#define roodWater   PB3 // digital 50

#define openLicht   PB4 // digital 10
#define dichtLicht  PB5 // digital 11

#define zoomerPin   PB6 // digital 12

int buttons[] = {stopLaag, stopHoog, knopOpen, knopDicht};
int lights[] = {groenWeg, roodWeg, roodWater, groenWater, dichtLicht, openLicht};

int brugOpen = 0;
int counter = 0;

void resetLichten() {
    for (int i = 0; i < 6; i += 2) {
        PORTB |= (1 << lights[i]);
    }

    for (int i = 1; i < 6; i += 2) {
        PORTB &= ~(1 << lights[i]);
    }
}

void init() {
    init_h_bridge();

    for (int i = 0; i < 4; i++) {
        DDRF &= ~(1 << buttons[i]);
    }


    for (int i = 0; i < 6; i++) {
        DDRB |= (1 << lights[i]);
    }
    DDRB |= (1 << zoomerPin);

    resetLichten();
}

int main(void) {
    init();

    while(1) {
        if ((PINF & (1 << knopOpen)) && brugOpen == 0) {
            PORTB |= (1 << roodWeg);
            PORTB &= ~(1 << groenWeg);
            PORTB &= ~(1 << dichtLicht);

            counter = 0;
            while (counter < 10) {
                PORTB ^= (1 << zoomerPin);
                PORTB ^= (1 << openLicht);
                PORTB ^= (1 << groenWater);
                _delay_ms(250);

                counter++;
            }

            h_bridge_set_percentage(-50);
            while (brugOpen == 0) {
                PORTB ^= (1 << openLicht);
                PORTB ^= (1 << groenWater);

                if (PINF & (1 << stopHoog)) {
                    brugOpen = 1;
                }

                _delay_ms(250);
            }
            h_bridge_set_percentage(0);

            PORTB |= (1 << openLicht);
            PORTB &= ~(1 << dichtLicht);
            PORTB |= (1 << groenWater);
            PORTB &= ~(1 << roodWater);
        }

        if ((PINF & (1 << knopDicht)) && brugOpen == 1) {
            PORTB |= (1 << roodWater);
            PORTB &= ~(1 << groenWater);
            PORTB &= ~(1 << openLicht);

            h_bridge_set_percentage(50);
            while (brugOpen == 1) {
                PORTB ^= (1 << dichtLicht);

                if (PINF & (1 << stopLaag)) {
                    brugOpen = 0;
                }

                _delay_ms(250);
            }
            h_bridge_set_percentage(0);

            counter = 0;
            while (counter < 4) {
                PORTB ^= (1 << zoomerPin);
                PORTB ^= (1 << dichtLicht);
                _delay_ms(250);

                counter++;
            }

            PORTB |= (1 << dichtLicht);
            PORTB &= ~(1 << openLicht);
            PORTB |= (1 << groenWeg);
            PORTB &= ~(1 << roodWeg);
        }
    }

    return 0;
}
