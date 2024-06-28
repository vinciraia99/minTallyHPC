#include "tally.hpp"
#include "simulation.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <iostream>

Tally *initialize_tallies(Parameters *params)
{
  // Ottieni il seed RNG
  uint64_t seed = Random::init_seed(0);

  Tally *tallies = static_cast<Tally *>(std::malloc(params->n_tallies * sizeof(Tally))); // sycl malloc sharwd

  for (int i = 0; i < params->n_tallies; ++i)
  {
    tallies[i].n_scores = params->n_nuclides * params->n_scores;
    tallies[i].n_filter_bins = params->n_filter_bins;
    tallies[i].filter_bins = static_cast<int *>(std::malloc(params->n_filter_bins * sizeof(int)));

    for (int j = 0; j < params->n_filter_bins; ++j)
    {
      tallies[i].filter_bins[j] = rni(0, params->n_coord, &seed);
    }

    // Alloca spazio per ciascun bin del filtro e combinazione di punteggi e i tre tipi di risultati delle tally
    tallies[i].results = tensor3d(tallies[i].n_filter_bins, tallies[i].n_scores, 3);
  }
  return tallies;
}

void free_tallies(Tally *tallies, int n)
{
  for (int i = 0; i < n; ++i)
  {
    std::free(tallies[i].filter_bins);
    free_tensor3d(tallies[i].results);
  }
  std::free(tallies);
}
