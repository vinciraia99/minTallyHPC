#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include "parameters.hpp"
#include <sycl/sycl.hpp>

// Get the current time in seconds
double get_time();

// Compute the sum of array elements
float sum(float *a, unsigned long n);

// 2D matrix of doubles contiguous in memory initialized to zero
float** matrix(size_t m, size_t n, sycl::queue &q);

// 3D tensor of doubles contiguous in memory initialized to zero
float*** tensor3d(size_t l, size_t m, size_t n, sycl::queue &q);

// Free matrix memory
void free_matrix(float **matrix, sycl::queue &q);

// Free 3D tensor memory
void free_tensor3d(float ***tensor3d, sycl::queue &q);

// Calculate the memory footprint in GB of the tally data
float calculate_memory(Parameters *params);

#endif
