#ifndef AUDIOHASH_AUDIOHASH_H
#define AUDIOHASH_AUDIOHASH_H

#include "fft.h"

const unsigned MIN_FREQ = 50; // bark 0.5
const unsigned MAX_FREQ = 3400; // bark 16.5
const unsigned FILTS_COUNT = 33; // for 32-bit int

class HashCalculator
{
    // smooth window function
    double *window;

    // weight matrix for fftbin2barkbin
    double *wts[FILTS_COUNT];

    FFT fft;

    void calc_FFT_bin(float *buf, double *magnF);

    void calc_Bark_bin(double *magnF, double *bark_bins);

public:
    // the count of samples to generate a single hash value
    const unsigned FRAME_LENGTH;
    const unsigned HALF_FFT;

    // the step length each time move fft window
    const unsigned STEP_LENGTH;
    const float MINOR_CHANGE;

    HashCalculator(unsigned frame_length, unsigned step_length, unsigned sample_rate, float minor_change);

    ~HashCalculator();

    uint32_t *calcHash(float *buf, size_t N, size_t *nframes);
};

#endif
