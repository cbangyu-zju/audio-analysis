#include <jni.h>
#include <string>
#include "capi.h"
#include <algorithm>
#include <math.h>

using namespace std;


static float *byte_to_float(const char *decbuf, size_t nChar, int nChannel, int sampleResolution){
    size_t iPoint, nPoint, index = 0;
    size_t *nSample = 0;
    int iChannel;
    float *buffer = NULL;
    switch (sampleResolution)
    {
        case 16 :
            nPoint = nChar / sizeof(short);
            *nSample = nPoint / nChannel;
            buffer = new float[*nSample];
            for (iPoint = 0; iPoint < nPoint; iPoint += nChannel)
            {
                buffer[index] = 0.0f;
                for (iChannel = 0; iChannel < nChannel ; iChannel++)
                {
                    buffer[index] += (((short *)decbuf)[iPoint + iChannel]) / (float)32767;
                }
                buffer[index++] /= nChannel;
            }
            break;
        case 8:
            nPoint = nChar / sizeof(short);
            *nSample = nPoint / nChannel;
            buffer = new float[*nSample];
            for (iPoint = 0; iPoint < nPoint; iPoint += nChannel)
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
            nPoint = nChar / sizeof(short);
            *nSample = nPoint / nChannel;
            buffer = new float[*nSample];
            for (iPoint = 0; iPoint < nPoint; iPoint += nChannel)
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


extern "C" JNIEXPORT jfloat

JNICALL
Java_demo_rxjava_com_ndk_MainActivity_stringFromJNI(JNIEnv *env, jobject instance,
                                                    jbyteArray bytes_, jint size) {
    float *pDouble = byte_to_float(reinterpret_cast<const char *>(bytes_), size, 2, 16);
    void *config = NULL;
    int error = 0;//
    float score = 0;
    size_t nchannel = 1;
    size_t sample_rate = 16000;
    size_t nsample = 24000;
//    float buffer[24000] = {16};
//    fill_n(buffer, 24000, 16);
    config = new_audiohash_config();

    error = set_pattern_audio(config, nchannel, sample_rate, pDouble, nsample);
    if (0 != error)
    {
        delete_audiohash_config(config);
//        return -1;
    }

    score = audio_compare(config, nchannel, sample_rate, pDouble, nsample);

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
    return score;

//    std::string hello = "Hello from C++";
//    return env->NewStringUTF(hello.c_str());





}
