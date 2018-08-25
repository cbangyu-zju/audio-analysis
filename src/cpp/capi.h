#ifndef AUDIOHASH_CAPI_H
#define AUDIOHASH_CAPI_H

void *new_audiohash_config();

void delete_audiohash_config(void *config);

int set_pattern_audio(void *config,
                      size_t nchannel,
                      size_t sample_rate,
                      float *buffer,
                      size_t nsample);

float audio_compare(void *config,
                    size_t nchannel,
                    size_t sample_rate,
                    float *buffer,
                    size_t nsample);

#endif
