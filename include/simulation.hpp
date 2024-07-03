#ifndef SIMULATION_H
#define SIMULATION_H

#include "parameters.hpp"
#include "tally.hpp"
#include <sycl/sycl.hpp>

// Loop over particles and tally
unsigned long long  simulate(Parameters *params, Tally *tallies, sycl::queue &q);

// Sample the number of events each particle undergoes in its lifetime
int sample_n_events(Parameters *params, uint64_t *seed);

#endif
