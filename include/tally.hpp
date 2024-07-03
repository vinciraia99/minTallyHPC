#ifndef TALLY_HPP
#define TALLY_HPP

#include "parameters.hpp"
#include <sycl/sycl.hpp>

// Tally results
#define VALUE 0
#define SUM 1
#define SUM_SQ 2

typedef struct {
  // Number of physical quantities to be tallied, e.g. reaction rates
  int n_scores;

  // Filter combinations, e.g. cell and energy
  int n_filter_bins;
  int *filter_bins;

  // Tally results for each bin. The first dimension is the combination of
  // filters (e.g. specific cell). The second dimension is the score (e.g.
  // flux, reaction rate). The third dimension is for the three tally result
  // types: the tally value for this cycle, the accumulated sum over all
  // cycles, and the sum squared over all cycles.
  float*** results;
} Tally;

// Allocate memory for tallies
Tally* initialize_tallies(Parameters* params, sycl::queue& q);

// Free memory
void free_tallies(Tally* tallies, int n, sycl::queue& q);

#endif
