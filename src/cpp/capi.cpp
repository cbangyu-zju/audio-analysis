#include <cstdio>
#include <stdint.h>
#include "capi.h"
#include "compare.h"
#include "hashcalculator.h"
#include "audioreader.h"

using std::vector;

struct AudioHashConfig
{
    AudioReader *const reader;
    HashCalculator *const calculator;
    Compare *const compare;
    const float frame_time;
    const float step_time;

    AudioHashConfig(
        unsigned sample_rate,
        unsigned frame_length,
        unsigned step_length,
        float minor_change)
        : reader(new AudioReader(sample_rate))
        , calculator(new HashCalculator(frame_length, step_length, sample_rate, minor_change))
        , compare(new Compare())
        , frame_time(frame_length / (float)sample_rate)
        , step_time(step_length / (float)sample_rate)
    {
    }

    ~AudioHashConfig()
    {
        delete reader;
        delete calculator;
        delete compare;
    }
};

void *new_audiohash_config()
{
    return new AudioHashConfig(8000, 4096, 400, 0.001);
}

int set_pattern_audio(void *_config,
                      size_t nchannel,
                      size_t sample_rate,
                      float *buffer,
                      size_t nsample)
{
    int error = 0;
    size_t output_buffer_length, nframes;
    float *sigbuf = NULL;
    uint32_t *hash = NULL;
    AudioHashConfig *config = (AudioHashConfig *)_config;
    if(nsample / (nchannel * 0.5) <= sample_rate){
        fprintf(stderr, "Sample is too short, must longer than 500ms!");
        return -1.0;
    }
    try
    {
        sigbuf = config->reader->readAudio(&nchannel, &sample_rate, buffer, &nsample, &output_buffer_length);
        hash = config->calculator->calcHash(sigbuf, output_buffer_length, &nframes);
        config->compare->setPattern(hash, nframes);
    }
    catch (std::exception &e)
    {
        fprintf(stderr, "%s\n", e.what());
        error = -1;
    }
    catch (...)
    {
        fprintf(stderr, "Unexpected error\n");
        error = -1;
    }

    if (NULL != sigbuf)
    {
        delete sigbuf;
    }

    return error;
}

float audio_compare(void *_config,
                    size_t nchannel,
                    size_t sample_rate,
                    float *buffer,
                    size_t nsample)
{
    int error = 0;
    size_t output_buffer_length, nframes;
    float *sigbuf = NULL;
    uint32_t *hash = NULL;
    float score = 0.5;
    AudioHashConfig *config = (AudioHashConfig *)_config;
    if(nsample / (nchannel * 0.5) <= sample_rate){
        fprintf(stderr, "Sample is too short, must longer than 500ms!");
        return -1.0;
    }
    try
    {
        sigbuf = config->reader->readAudio(&nchannel, &sample_rate, buffer, &nsample, &output_buffer_length);
        hash = config->calculator->calcHash(sigbuf, output_buffer_length, &nframes);
        score = config->compare->compare(hash, nframes);
        return score;
    }
    catch (std::exception &e)
    {
        fprintf(stderr, "%s\n", e.what());
        error = -1;
    }
    catch (...)
    {
        fprintf(stderr, "Unexpected error\n");
        error = -1;
    }

    if (NULL != sigbuf)
    {
        delete sigbuf;
    }
    if (NULL != hash)
    {
        delete hash;
    }

    return score;
}

void delete_audiohash_config(void *config)
{
    delete (AudioHashConfig *)config;
}
