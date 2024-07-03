#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>
#include <sycl/sycl.hpp>

typedef struct {
    uint64_t seed;
    int n_threads;
    unsigned long n_particles;
    int n_tallies;
    int n_filter_bins;
    int n_scores;
    int n_nuclides;
    int n_coord;
    float n_events;
} Parameters;

// Set the default parameters and read CLI
Parameters *initialize_parameters(int argc, char *argv[], sycl::queue &q);

// Free memory
void free_parameters(Parameters *params,sycl::queue &q);

#endif
