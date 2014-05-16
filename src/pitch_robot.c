#include <avr/io.h>
#include <avr/interrupt.h>

#define SIGN(a) ((a) > 127)
#define MAG(a) ((a) > 127 ? a - 127 : 127 - a)

#define SIGNAL_FLOOR 35
#define MIN_PERIOD 9
#define MAX_PERIOD 1000
#define CANTIDATES 15
#define VOTES 25
#define SAMPLE_RATE 9315
#define MOTOR_MASK 0b00001111
#define LED_MASK 0b00010000

#define STOP 0b1010
#define GO 0b0101
#define REVERSE 0b0000
#define LEFT 0b0001
#define RIGHT 0b0100

typedef unsigned char Sample;

typedef enum {E2, F2, G2, A3, B3, C3, D3, E3, F3, G3, A4, B4, C4, D4, E4, ERROR} Note;

volatile Sample s, prev;
volatile int period, timer, old_period;
volatile Sample max_amp;
int votes[CANTIDATES];
int state;

void move_tekbot(int pitch);

ISR(ADC_vect) {
    s = ADCH;

    if (++timer < MIN_PERIOD) { // we are too close to a previous zero crossing
        // do nothing
    } else if (timer >= MAX_PERIOD) { // we are too far from a previous zero crossing
        old_period = 0;
        period = -1;
        timer = 0;
    } else if (SIGN(s) && !SIGN(prev)) { // we have a positive zero crossing
        if (old_period > 0) // don't modify period after the first zero crossing
            period = timer;
        old_period = timer;
        timer = 0;
    } else { // we are continuing analysis normally
        period = 0;
    }

    // record the maximum amplitude so we can determine if
    // the signal is strong enough
    if (MAG(s) > max_amp)
        max_amp = MAG(s);

    prev = s;
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

    sei();                        // enable interrupts

    // main loop
    for(;;) {
        if (period) {
            move_tekbot(period);
        }
    }
}

Note get_pitch(freq)
{
    if (freq > 73 && freq < 85)
        return E2;
    else if (freq < 93)
        return F2;
    else if (freq < 104)
        return G2;
    else if (freq < 116)
        return A3;
    else if (freq < 127)
        return B3;
    else if (freq < 138)
        return C3;
    else if (freq < 155)
        return D3;
    else if (freq < 169)
        return E3;
    else if (freq < 185)
        return F3;
    else if (freq < 207)
        return G3;
    else if (freq < 233)
        return A4;
    else if (freq < 254)
        return B4;
    else if (freq < 277)
        return C4;
    else if (freq < 311)
        return D4;
    else if (freq < 349)
        return E4;
    else
        return ERROR;
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

void move_tekbot(int p)
{
    Note pitch;
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
    if (max_amp < SIGNAL_FLOOR) {
        PORTB = STOP;
        return;
    }

    pitch = get_pitch(SAMPLE_RATE / p);

    // if the signal is outside our range of pitches, continue
    if (pitch == ERROR) {
        return;
    }

    // otherwise, we are getting a valid signal, so pick a command
    // based on the pitch
    if (++(votes[pitch]) > VOTES) {
        switch (pitch) {
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
            break;
        }

        // once a command has been selected, start the voting again
        for (i = 0; i < CANTIDATES; i++) {
            votes[i] = 0;
        }

    // Close encounters: 196, 220, 175, 87, 131
    }

    return;
}
