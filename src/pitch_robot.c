#include <avr/io.h>
#include <avr/interrupt.h>
#include "pitch_analyzer.h"

#define CANTIDATES 15
#define VOTES 25
#define MOTOR_MASK 0b00001111
#define LED_MASK 0b00010000

#define SAMPLE_RATE 9315

#define STOP 0b1010
#define GO 0b0101
#define REVERSE 0b0000
#define LEFT 0b0001
#define RIGHT 0b0100

void move_tekbot(int period);
void secret_dance(void);

int votes[CANTIDATES]; // vote on cantidate pitches to reduce errant commands
int state; // state machine for detection of 5 note pattern
volatile int period;
PitchContext c;

ISR(ADC_vect) {
    period = pitch_sample(ADCH, &c);
}

int main(void) {

    /*
     * ADC Setup (page 101 of datasheet)
     *
     * ADMUX:
     * allow reading the 8 highest bits directly from ADCH (ADLAR)
     *
     * ADCSRA:
     * enable free running mode (ADATE)
     * enable adc interrupts (ADIE)
     * enable the ADC itself (ADEN)
     * set the adc prescaler to 8 (ADPS)
     */

    ADMUX = _BV(ADLAR);
    ADCSRA = _BV(ADATE) | _BV(ADIE) | _BV(ADEN) | _BV(ADPS1) | _BV(ADPS0);

    DIDR0 = 0xff;                 // disable digital input buffers in PORTA
    DDRB = MOTOR_MASK | LED_MASK; // set port b to outputs for moving the Tekbot
    PORTB = STOP;                 // stop the tekbot

    ADCSRA |= _BV(ADSC);          // begin reading from ADC in free running mode

    pitch_init(&c);

    sei();                        // enable interrupts

    // main loop
    for(;;) {
        if (period) {
            move_tekbot(period);
        }
    }
}


void secret_dance(void)
{
    unsigned int i;

    cli();

    for(i = 0; i < 60000; i++) {
        PORTB = 0b10110;
    }

    for(i = 0; i < 60000; i++) {
        PORTB = 0b11001;
    }

    PORTB = STOP;

    sei();
}

void move_tekbot(int period)
{
    Note note;
    int i;

    // if too much time elapsed between zero crossings, stop and reset
    // the votes
    if (period < 0) {
        for (i = 0; i < CANTIDATES; i++) {
            votes[i] = 0;
        }
        PORTB = STOP;
        return;
    }

    // if the signal is not big enough, stop the tekbot
    if (pitch_get_peak_amp(&c) < SIGNAL_FLOOR) {
        PORTB = STOP;
        return;
    }

    note = pitch_get_note(period, SAMPLE_RATE);

    // if the signal is outside our range of pitches, continue
    if (note == ERROR) {
        return;
    }

    // otherwise, we are getting a valid signal, so pick a command
    // based on the pitch
    if (++(votes[note]) > VOTES) {
        switch (note) {
        case E2:
            PORTB = REVERSE;
            state = 0;
            break;
        case A3:
            PORTB = GO;
            state = 0;
            break;
        case D3:
            PORTB = LEFT;
            state = 0;
            break;
        case G3:
            PORTB = RIGHT;
            state = 1;
            break;
        case A4:
            if (state == 1 || state == 2)
                state = 2;
            else
                state = 0;
            PORTB = STOP;
            break;
        case F3:
            if (state == 2 || state == 3)
                state = 3;
            else
                state = 0;
            PORTB = STOP;
            break;
        case F2:
            if (state == 3 || state == 4)
                state = 4;
            else
                state = 0;
            PORTB = STOP;
            break;
        case C3:
            if (state == 4)
                secret_dance();
            state = 0;
            PORTB = STOP;
            break;
        default:
            PORTB = STOP;
            state = 0;
            break;
        }

        // once a command has been selected, start the voting again
        for (i = 0; i < CANTIDATES; i++) {
            votes[i] = 0;
        }

    // Close Encounters theme: G3 A4 F3 F2 C3
    }

    return;
}
