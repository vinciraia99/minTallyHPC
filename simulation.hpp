#ifndef SIMULATION_H
#define SIMULATION_H

#include <sycl/sycl.hpp>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include "parameters.hpp"
#include "tally.hpp"

using namespace sycl;

// Dichiarazioni delle costanti LCG
constexpr uint64_t mult = 19073486328125ULL;  // Moltiplicatore, g
constexpr uint64_t mod = 281474976710656ULL;  // Modulo, 2^M
constexpr uint64_t inc = 1ULL;                // Incremento, c
constexpr uint64_t mask = 281474976710655ULL; // Maschera, 2^M - 1

// Funzione per generare numeri casuali
float rn(uint64_t *seed);

// Funzione per generare numeri interi casuali in un intervallo
int rni(int a, int b, uint64_t *seed);

// Classe Random per la gestione dei semi casuali
class Random
{
public:
  static void set_seed(uint64_t seed)
  {
    master_seed = seed;
  }

  static uint64_t init_seed(int64_t id)
  {
    const uint64_t stride = 152917ULL;
    return future_seed(id * stride, master_seed);
  }

  static void advance_seed(int64_t n, uint64_t *seed)
  {
    *seed = future_seed(n, *seed);
  }

  static uint64_t get_master_seed()
  {
    return master_seed;
  }

  static uint64_t future_seed(uint64_t n, uint64_t seed)
  {
    uint64_t g = mult;
    uint64_t c = inc;
    uint64_t g_new = 1;
    uint64_t c_new = 0;

    n &= mask;

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

private:
  static int64_t master_seed;
};

// Funzione per campionare da una distribuzione gaussiana
float sample_gaussian(float mu, float sigma, uint64_t *seed);

// Funzione di simulazione principale
float simulate(Parameters *params, Tally *tallies, uint64_t master_seed);

// Funzione per campionare il numero di eventi
int sample_n_events(Parameters *params, uint64_t *seed);

#endif // SIMULATION_H
