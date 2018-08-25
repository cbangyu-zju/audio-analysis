#include <cstdio>


#include "compare.h"
#include <math.h>

using std::vector;

const uint32_t MASK_01010101 = (((uint32_t)(-1)) / 3);
const uint32_t MASK_00110011 = (((uint32_t)(-1)) / 5);
const uint32_t MASK_00001111 = (((uint32_t)(-1)) / 17);

float Compare::array_compare(const uint32_t *hashA, const uint32_t *hashB, size_t size){
    float dot = 0.0, denom_a = 0.0, denom_b = 0.0 ;
    for(unsigned int i = 0u; i < size; ++i) {
        dot += hashA[i] * hashB[i] ;
        denom_a += hashA[i] * hashA[i] ;
        denom_b += hashB[i] * hashB[i] ;
    }
    return dot / (sqrt(denom_a) * sqrt(denom_b)) ;
}

void Compare::setPattern(uint32_t *hash, size_t nframes)
{
    pattern.hash = hash;
    pattern.nframes = nframes;
}

float Compare::compare(const uint32_t *hash, size_t nframes)
{
    size_t compare_nframes = pattern.nframes>=nframes?nframes:pattern.nframes;
    float score = array_compare(hash, pattern.hash, compare_nframes);
    return score;
}
