#ifndef AUDIOHASH_FFT_H
#define AUDIOHASH_FFT_H

#include <complex.h>

using namespace std;

class FFT
{
    complex <double> *Xt;
    complex <double> *twiddle_factors;

public:
    const int FFT_WINDOW;
    const int HALF_FFT;
    FFT(int window_size);
    ~FFT();

    void fft(const double *x, complex <double> *X);
};

#endif
