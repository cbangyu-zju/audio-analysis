#ifndef AUDIOHASH_COMPARE_H
#define AUDIOHASH_COMPARE_H

#include <vector>

struct HashPattern
{
    uint32_t *hash;
    size_t nframes;
};

class Compare{

public:
    HashPattern pattern;
    void setPattern(uint32_t *hash, size_t nframes);
    float compare(const uint32_t *hash, size_t nframes);
    float array_compare(const uint32_t *hashA, const uint32_t *hashB, size_t size);
};

#endif
