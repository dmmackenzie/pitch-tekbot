#include "pitch_analyzer.h"

void pitch_init(PitchContext* c)
{
    c->prev = 128;
    c->peak_amp = 0;
    c->timer = 0;
    c->old_period = 0;
}

Sample pitch_get_peak_amp(PitchContext* c)
{
    return c->peak_amp;
}

int pitch_sample(Sample s, PitchContext* c)
{
    int period;

    c->timer += 1;

    if (c->timer < MIN_PERIOD) { // we are too close to a previous zero crossing
        period = 0;
    } else if (c->timer >= MAX_PERIOD) { // we are too far from a previous zero crossing
        period = -1;
        c->old_period = 0;
        c->peak_amp = 0;
        c->timer = 0;
    } else if (SIGN(s) && !SIGN(c->prev)) { // we have a positive zero crossing
        if (c->old_period > 0) // don't return a valid period if we've only had one zero crossing
            period = c->timer;
        c->old_period = c->timer;
        c->peak_amp = 0;
        c->timer = 0;
    } else { // we are continuing analysis normally
        period = 0;
    }

    // record the maximum amplitude so we can determine if
    // the signal is strong enough
    if (MAG(s) > c->peak_amp)
        c->peak_amp = MAG(s);

    c->prev = s;

    return period;
}

Note pitch_get_note(int period, int sample_rate)
{
    int freq = sample_rate / period;

    if (freq < 73)
        return ERROR;
    else if (freq < 85)
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
