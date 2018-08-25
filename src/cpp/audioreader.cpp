#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <climits>


extern "C"{

//#include "samplerate.h"

}
#include <memory>
#include "audioreader.h"

using std::auto_ptr;

static void readAudioPCM(
    size_t nchannel,
    float *input_buffer,
    float *output_buffer,
    size_t nsample
)
{
    size_t iChannel, iPoint, index = 0;
    for (iPoint = 0; iPoint < nsample; iPoint += nchannel)
    {
        output_buffer[index] = 0.0f;
        for (iChannel = 0; iChannel < nchannel ; iChannel++)
        {
            output_buffer[index] += fabs((input_buffer)[iPoint + iChannel]);
        }
        output_buffer[index++] /= nchannel;
    }
}

float *AudioReader::readAudio(
    size_t *nchannel,             // 声道个数
    size_t *sample_rate,          // 采样时间分辨率
    float *buffer,        // 采样流数据
    size_t *nsample,              // 采样点数
    size_t *output_buffer_length) // 输出流数据长度
{
    long dist_sample_rate = SAMPLE_RATE;

    float *input_buffer = new float[(*nsample)];;
    readAudioPCM(*nchannel, buffer, input_buffer, *nsample);
    if (dist_sample_rate == *sample_rate)
    {
        *output_buffer_length = *nsample;
        return input_buffer;
    }

    auto_ptr<float> inbuffer_auto(input_buffer);

    /* resample float array */
    /* set distribution sample rate ratio */
    double sr_ratio = (double)(dist_sample_rate) / (double)(*sample_rate);

    /* allocate output buffer for conversion */
    *output_buffer_length = (size_t)(sr_ratio * (*nsample));
    float *output_buffer = new float[int(*output_buffer_length)];
    auto_ptr<float> output_buffer_auto(output_buffer);

    int error;
    return output_buffer_auto.release();
}

AudioReader::AudioReader(unsigned sr)
    : SAMPLE_RATE(sr)
{
}

AudioReader::~AudioReader()
{
}
