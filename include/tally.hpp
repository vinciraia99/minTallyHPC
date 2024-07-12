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

  // Tally results for each bin stored in a monodimensional array.
  // The size will be n_filter_bins * n_scores * 3
  float* results;
} Tally;

// Allocate memory for tallies
Tally* initialize_tallies(Parameters* params, sycl::queue& q);

// Free memory
void free_tallies(Tally* tallies, int n, sycl::queue& q);

SYCL_EXTERNAL void increment_value_atomic(float* array, int n_filter_bins, int n_scores, int filter_bin, int score, int tally_type, float increment);

SYCL_EXTERNAL float get_value(float* array, int n_filter_bins, int n_scores, int filter_bin, int score, int tally_type);

SYCL_EXTERNAL void set_value(float* array, int n_filter_bins, int n_scores, int filter_bin, int score, int tally_type, float value);

#endif
