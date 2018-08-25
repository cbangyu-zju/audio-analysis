#include <cmath>

#include <complex.h>
#include <stdbool.h>
#include "fft.h"

static
complex <double> polar_to_complex(const double r, const double theta)
{
    complex <double> result;
    result.real(r * cos(theta));
    result.imag(r * sin(theta));
    return result;
}

static
void fft_calc(const int N, const double *x, complex <double> *X, complex <double> *P, const int step, const complex <double> *twids)
{
    int k;
    complex <double> *S = P + N / 2;
    if (N == 1)
    {
        X[0] = x[0];
        return;
    }

    fft_calc(N / 2, x,      S,   X, 2 * step, twids);
    fft_calc(N / 2, x + step, P,   X, 2 * step, twids);

    for (k = 0; k < N / 2; k++)
    {
        P[k] = P[k] * twids[k * step];
        X[k]     = S[k] + P[k];
        X[k + N / 2] = S[k] - P[k];
    }
}

FFT::FFT(int window_size)
    : FFT_WINDOW(window_size), HALF_FFT(window_size >> 1)
{
    Xt = new complex <double>[FFT_WINDOW];
    twiddle_factors = new complex <double>[HALF_FFT];

    for (int k = 0; k < FFT_WINDOW / 2; k++)
    {
        twiddle_factors[k] = polar_to_complex(1.0, 2.0 * M_PI * k / FFT_WINDOW);
    }
}

FFT::~FFT()
{
    delete Xt;
    delete twiddle_factors;
}

void FFT::fft(const double *x, complex <double> *X)
{
    fft_calc(FFT_WINDOW, x, X, Xt, 1, twiddle_factors);
}
