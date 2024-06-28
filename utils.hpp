#ifndef UTILS_H
#define UTILS_H

#include "parameters.hpp"
#include <cstddef> // per std::size_t

// Get the current time in seconds
double get_time();

// Compute the sum of array elements
float sum(float *a, unsigned long n);

// 2D matrix of doubles contiguous in memory initialized to zero
float **matrix(size_t m, size_t n);

// 3D tensor of doubles contiguous in memory initialized to zero
float ***tensor3d(size_t l, size_t m, size_t n);

// Free matrix memory
void free_matrix(float **matrix);

// Free 3D tensor memory
void free_tensor3d(float ***tensor3d);

// Calculate the memory footprint in GB of the tally data
float calculate_memory(const Parameters *params);

#endif
