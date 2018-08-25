#include <cstdlib>
#include <cmath>
#include <cstring>
#include <stdint.h>
#include <stdbool.h>
#include "hashcalculator.h"
#include "fft.h"

static double hz2bark(double f)
{
    double tmp = (26.81 * f / (1960 + f)) - 0.53;
    if (tmp < 2)
    {
        tmp += 0.15 * (2 - tmp);
    }
    else if (tmp > 20.1)
    {
        tmp += 0.22 * (tmp - 20.1);
    }

    return tmp < 0 ? 0 : tmp;
}

HashCalculator::HashCalculator(unsigned frame_length, unsigned step_length, unsigned sample_rate, float minor_change)
    : fft(frame_length), FRAME_LENGTH(frame_length), HALF_FFT(frame_length >> 1), STEP_LENGTH(step_length), MINOR_CHANGE(minor_change)
{
    // allocate buf
    window = new double[FRAME_LENGTH];
    for (unsigned i = 0; i < FILTS_COUNT; i++)
    {
        wts[i] = new double[HALF_FFT];
    }

    // init_hamming_window()
    for (unsigned i = 0; i < FRAME_LENGTH; i++)
    {
        window[i] = 0.54 - 0.46 * cos(2 * M_PI * i / (FRAME_LENGTH - 1));
    }

    // init_wts()
    double minbark = hz2bark(MIN_FREQ);
    double maxbark = hz2bark(MAX_FREQ);
    double nyqbark = maxbark - minbark;
    double stepbarks = nyqbark / (FILTS_COUNT - 1);

    double lof, hif;
    double binbarks[HALF_FFT];
    for (unsigned i = 0; i < HALF_FFT; i++)
    {
        binbarks[i] = hz2bark((double)i * sample_rate / FRAME_LENGTH);
    }

    for (unsigned i = 0; i < FILTS_COUNT; i++)
    {
        double f_bark_mid = minbark + i * stepbarks;
        for (unsigned j = 0; j < HALF_FFT; j++)
        {
            double barkdiff = binbarks[j] - f_bark_mid;
            lof = -2.5 * (barkdiff - 0.5);
            hif = barkdiff + 0.5;
            double m = fmin(lof, hif);
            m = fmin(0.0, m);
            m = pow(10, m);
            wts[i][j] = m;
        }
    }
}

HashCalculator::~HashCalculator()
{
    for (unsigned i = 0; i < FILTS_COUNT; i++)
    {
        delete wts[i];
    }
    delete window;
}

void HashCalculator::calc_FFT_bin(float *buf, double *magnF)
{
    // the 2 arrays require 96k in stack, consider moving them to heap later
    double frame[FRAME_LENGTH];

    complex <double> pF[FRAME_LENGTH];

    for (unsigned i = 0; i < FRAME_LENGTH; i++)
    {
        frame[i] = window[i] * buf[i];
    }

    fft.fft(frame, pF);

    for (unsigned i = 0; i < HALF_FFT; i++)
    {
        magnF[i] = abs(pF[i]);
    }
}

void HashCalculator::calc_Bark_bin(double *magnF, double *bark_bins)
{
    for (unsigned i = 0; i < FILTS_COUNT; i++)
    {
        bark_bins[i] = 0;
        for (unsigned j = 0; j < HALF_FFT; j++)
        {
            bark_bins[i] += wts[i][j] * magnF[j];
        }
    }
}

uint32_t *HashCalculator::calcHash(float *buf, size_t N, size_t *nframes)
{
    double magnF[HALF_FFT], bark_bins[FILTS_COUNT];

    double prev_diffs[FILTS_COUNT - 1], curr_diffs[FILTS_COUNT - 1];

    *nframes = 1 + (N - FRAME_LENGTH) / STEP_LENGTH;

    uint32_t *hash = new uint32_t[*nframes];

    for (unsigned i = 0; i < FILTS_COUNT - 1; i++)
    {
        prev_diffs[i] = 0.0;
    }

    for (unsigned index = 0, start = 0, end = FRAME_LENGTH;
            end <= N;
            index += 1, start += STEP_LENGTH, end += STEP_LENGTH)
    {
        calc_FFT_bin(buf + start, magnF);

        calc_Bark_bin(magnF, bark_bins);

        // calc hash for 32 bit
        double curr_max_diff = 0;
        for (unsigned m = 0; m < FILTS_COUNT - 1; m++)
        {
            curr_diffs[m] = bark_bins[m] - bark_bins[m + 1];
            curr_max_diff = fmax(curr_max_diff, curr_diffs[m]);
        }

        uint32_t curr_hash = 0;
        for (unsigned m = 0; m < FILTS_COUNT - 1; m++)
        {
            if (curr_diffs[m] - prev_diffs[m] > MINOR_CHANGE * curr_max_diff) // ignore minor change
            {
                curr_hash |= 1 << m;
            }

            prev_diffs[m] = curr_diffs[m];
        }

        hash[index] = curr_hash;
    }
    return hash;
}
