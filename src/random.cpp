#include "random.hpp"
#include <sycl/sycl.hpp>

// LCG parameters from 'The MCNP5 Random Number Generator', Forrest Brown
const uint64_t mult = 19073486328125ULL;  // Multiplier, g
const uint64_t mod = 281474976710656ULL;  // Modulus, 2^M
const uint64_t inc = 1ULL;                // Increment, c
const uint64_t mask = 281474976710655ULL; // Mask, 2^M - 1

SYCL_EXTERNAL float rn(uint64_t *seed)
{
  *seed = (mult * (*seed) + inc) & mask;
  return static_cast<float>(*seed) / static_cast<float>(mod);
}

SYCL_EXTERNAL int rni(int a, int b, uint64_t *seed)
{
  *seed = (mult * (*seed) + inc) & mask;
  return a + (int)(b * (*seed) / (mod + a));
}

SYCL_EXTERNAL uint64_t init_seed(int64_t id, int64_t master_seed)
{
  const uint64_t stride = 152917ULL;
  return future_seed(id * stride, master_seed);
}

void advance_seed(int64_t n, uint64_t *seed) { *seed = future_seed(n, *seed); }

uint64_t future_seed(uint64_t n, uint64_t seed)
{
  uint64_t g = mult;
  uint64_t c = inc;
  uint64_t g_new = 1;
  uint64_t c_new = 0;

  // n = n % mod
  n &= mask;

  // Get mult = mult^n in log2(n) operations
  while (n > 0)
  {
    if (n & 1)
    {
      g_new *= g;
      c_new = c_new * g + c;
    }
    c *= g + 1;
    g *= g;
    n >>= 1;
  }

  return (g_new * seed + c_new) & mask;
}

SYCL_EXTERNAL float sample_gaussian(float mu, float sigma, uint64_t *seed)
{
  float theta = 2.0f * M_PI * rn(seed);
  float r = sycl::sqrt(-2.0f * sycl::log(rn(seed)));
  return r * sycl::cos(theta) * sigma + mu;
}
