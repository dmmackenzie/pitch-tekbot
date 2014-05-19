#ifndef PITCH_ANALYZER_H
#define PITCH_ANALYZER_H

#define SIGN(a) ((a) > 127)
#define MAG(a) ((a) > 127 ? a - 127 : 127 - a)

#define SIGNAL_FLOOR 35
#define MIN_PERIOD 9
#define MAX_PERIOD 1000

typedef unsigned char Sample;

typedef struct PitchContext {
    volatile Sample prev;
    volatile Sample peak_amp;
    volatile int timer, old_period;
} PitchContext;

typedef enum Note {
    E2, F2, G2, A3, B3, C3, D3,E3, F3, G3, A4, B4, C4, D4, E4,
    F4, G4, A5, B5, C5, D5, ERROR
} Note;

/*
 * pitch_init takes a pointer to a context object
 * The user is responsible for allocating space for the context object.
 * It should be called before performing any pitch analysis.
 * This object will be passed to other pitch analysis functions.
 */
void pitch_init(PitchContext* c);

/*
 * pitch_get_note is a helper function that returns a Note value when given a
 * period recieved from pitch_sample and the sample rate of the data.
 */
Note pitch_get_note(int period, int sample_rate);

/*
 * pitch_sample should be called whenever a new sample is taken.
 * pitch_sample returns E_PERIOD_OVERFLOW if it has been too long between
 * zero crossings. When pitch_sample returns 0, it is continuing analysis
 * normally. pitch_sample returns a positive integer if it has detected
 * a valid pitch.
 */
int pitch_sample(Sample s, PitchContext* c);

/*
 * pitch_get_peak_amp returns the peak amplitude of the signal. It should be
 * used to differentiate low amplitude noise from an actual signal.
 */
Sample pitch_get_peak_amp(PitchContext* c);

#endif //include guard
