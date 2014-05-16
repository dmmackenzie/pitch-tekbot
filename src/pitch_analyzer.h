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

typedef enum Note {E2, F2, G2, A3, B3, C3, D3,E3, F3, G3, A4, B4, C4, D4, E4,
                   F4, G4, A5, B5, C5, D5, ERROR} Note;

Note pitch_get_note(int period, int sample_rate);
int pitch_sample(Sample s, PitchContext* c);
void pitch_init(PitchContext* c);
Sample pitch_get_peak_amp(PitchContext* c);

#endif //include guard
