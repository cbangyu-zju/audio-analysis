#include <stdio.h>
#include <string.h>
#include <math.h>
#include "capi.h"
#include <algorithm>

extern "C"{
#include <mpg123.h>
}

using namespace std;

void printHelp()
{
    fprintf(stderr, "compare audio \n");
}

static float *byte_to_float(const char *decbuf, size_t nChar, int nChannel, int sampleResolution, size_t *nSample){
    size_t iPoint, index = 0;
    int iChannel;
    float *buffer = NULL;
    switch (sampleResolution)
    {
        case 16 :
            *nSample = nChar / sizeof(short);
            buffer = new float[*nSample];
            for (iPoint = 0; iPoint < *nSample; iPoint += nChannel)
            {
                buffer[index] = 0.0f;
                for (iChannel = 0; iChannel < nChannel ; iChannel++)
                {
                    buffer[index] += abs(((short *)decbuf)[iPoint + iChannel]) / (float)32767;
                }
                buffer[index++] /= nChannel;
            }
            break;
        case 8:
            *nSample = nChar / sizeof(char);
            buffer = new float[*nSample];
            for (iPoint = 0; iPoint < *nSample; iPoint += nChannel)
            {
                buffer[index] = 0.0f;
                for (iChannel = 0; iChannel < nChannel ; iChannel++)
                {
                    buffer[index] += abs(((char *)decbuf)[iPoint + iChannel]) / (float)127;
                }
                buffer[index++] /= nChannel;
            }
            break;
        case 32:
            *nSample = nChar / sizeof(float);
            buffer = new float[*nSample];
            for (iPoint = 0; iPoint < *nSample; iPoint += nChannel)
            {
                buffer[index] = 0.0f;
                for (iChannel = 0; iChannel < nChannel; iChannel++)
                {
                    buffer[index] += fabsf(((float *)decbuf)[iPoint + iChannel]);
                }
                buffer[index++] /= nChannel;
            }
            break;
        default:
            *nSample = 0;
    }
    return buffer;

}

static
float *readaudio_mp3(
        const char *filename,
        long *sr,
        const float nbsecs,
        size_t *buflen)
{
    mpg123_handle *m_handle;
    int ret;

    if (mpg123_init() != MPG123_OK || (m_handle = mpg123_new(NULL, &ret)) == NULL ||
        mpg123_open(m_handle, filename) != MPG123_OK)
    {
        printf("MPG123 error");
    }

    /*turn off logging */
    mpg123_param(m_handle, MPG123_ADD_FLAGS, MPG123_QUIET, 0);

    size_t totalsamples;

    mpg123_scan(m_handle);
    totalsamples = mpg123_length(m_handle);

    int iChannel, nChannel, encoding;
    if (mpg123_getformat(m_handle, sr, &nChannel, &encoding) != MPG123_OK)
    {
        printf("Bad format");
    }

    mpg123_format_none(m_handle);
    mpg123_format(m_handle, *sr, nChannel, encoding);

    size_t decbuflen = mpg123_outblock(m_handle);

    unsigned char *decbuf = new unsigned char[decbuflen];
    auto_ptr<unsigned char> decbuf_auto(decbuf);

    size_t nbsamples = (nbsecs <= 0) ? totalsamples : nbsecs * (*sr);
    nbsamples = (nbsamples < totalsamples) ? nbsamples : totalsamples;

    size_t iPoint, nPoint, index = 0, done;

    float *buffer = new float[nbsamples];
    auto_ptr<float> buffer_auto(buffer);

    *buflen = nbsamples;

    do
    {
        ret = mpg123_read(m_handle, decbuf, decbuflen, &done);
        //for(int read_i = 0; read_i < done; read_i++){printf("%d\n", abs(((char *)decbuf)[read_i]));}
        switch (encoding)
        {
            case MPG123_ENC_SIGNED_16 :
                nPoint = done / sizeof(short);
                for (iPoint = 0; iPoint < nPoint; iPoint += nChannel)
                {
                    buffer[index] = 0.0f;
                    for (iChannel = 0; iChannel < nChannel ; iChannel++)
                    {
                        buffer[index] += abs(((short *)decbuf)[iPoint + iChannel]) / (float)32767;
                    }
                    buffer[index++] /= nChannel;
                    if (index >= nbsamples) break;
                }
                break;
            case MPG123_ENC_SIGNED_8:
                nPoint = done / sizeof(char);
                for (iPoint = 0; iPoint < nPoint; iPoint += nChannel)
                {
                    buffer[index] = 0.0f;
                    for (iChannel = 0; iChannel < nChannel ; iChannel++)
                    {
                        buffer[index] += abs(((char *)decbuf)[iPoint + iChannel]) / (float)127;
                    }
                    buffer[index++] /= nChannel;
                    if (index >= nbsamples) break;
                }
                break;
            case MPG123_ENC_FLOAT_32:
                nPoint = done / sizeof(float);
                for (iPoint = 0; iPoint < nPoint; iPoint += nChannel)
                {
                    buffer[index] = 0.0f;
                    for (iChannel = 0; iChannel < nChannel; iChannel++)
                    {
                        buffer[index] += fabsf(((float *)decbuf)[iPoint + iChannel]);
                    }
                    buffer[index++] /= nChannel;
                    if (index >= nbsamples) break;
                }
                break;
            default:
                done = 0;
        }

    }
    while (ret == MPG123_OK && index < nbsamples);

    mpg123_close(m_handle);
    mpg123_delete(m_handle);
    mpg123_exit();

    return buffer_auto.release();
}

int main(int argc, char **argv)
{
    void *config = NULL;
    int error = 0;
    double score = 0;

    // init default wave
    //size_t nchannel = 1;
    //size_t sample_rate = 8000;
    //size_t nsample = 80000;
    //float buffer[80000] = {0.1};
    //fill_n(buffer, 80000, 0.1);
    config = new_audiohash_config();

    // read standard redio
    size_t std_buffer_length;
    long std_orig_sr;
    size_t std_nchannel = 1;
    const char *std_filename = "/Users/caibangyu/VSCode/audiohash/music/standard.mp3";
    float *std_buffer;
    std_buffer = readaudio_mp3(std_filename, &std_orig_sr, 0, &std_buffer_length);

    // read compare redio
    size_t compare_buffer_length;
    long compare_orig_sr;
    size_t compare_nchannel = 1;
    const char *compare_filename = "/Users/caibangyu/VSCode/audiohash/music/compare.mp3";
    float *compare_buffer;
    compare_buffer = readaudio_mp3(compare_filename, &compare_orig_sr, 0, &compare_buffer_length);

    error = set_pattern_audio(config, std_nchannel, (size_t)std_orig_sr, std_buffer, std_buffer_length);
    if (0 != error)
    {
        delete_audiohash_config(config);
        return -1;
    }
    printf("compare!\n");

    score = audio_compare(config, compare_nchannel, (size_t)std_orig_sr, compare_buffer, compare_buffer_length);

    /*
    if (0 == score)
    {
        fprintf(stderr, "Error occurs while compare pattern");
    }
    else
    {
        printf("socre: %8.4lf\n", score);
    }
    */
    printf("score: %f\n", score);
    return 0;
}