#ifndef RANDOM_H
#define RANDOM_H

#ifndef M_PI
    #define M_PI 3.14159265358979323846f
#endif

#include <stdint.h>
#include <sycl/sycl.hpp>

// Linear congruential random number generator: seed = (mult*seed + inc) % mod
SYCL_EXTERNAL float rn(uint64_t* seed);

// Sample a random integer in the range [a b)
SYCL_EXTERNAL int rni(int a, int b, uint64_t* seed);

// Get the inital seed for the given ID
SYCL_EXTERNAL uint64_t init_seed(int64_t id,int64_t master_seed);

// Algorithm to skip ahead n*rng.stride random numbers in O(log2(n)) operation,
// from 'The MCNP5 Random Number Generator', Forrest Brown, LA-UR-07K-7961.
void advance_seed(int64_t n, uint64_t* seed);

// Get the seed n skips ahead
uint64_t future_seed(uint64_t n, uint64_t seed);

// Sample a random number from a Gaussian distribution using Box-Muller
SYCL_EXTERNAL float sample_gaussian(float mu, float sigma, uint64_t *seed);

#endif
