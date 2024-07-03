#include "utils.hpp"
#include <cstdlib>
#include <chrono>
#include <sycl/sycl.hpp>

// Funzione per ottenere il tempo corrente
double get_time() {
    auto now = std::chrono::steady_clock::now();
    auto now_us = std::chrono::time_point_cast<std::chrono::microseconds>(now);
    auto epoch = now_us.time_since_epoch();
    return epoch.count() / 1000000.0;
}

float sum(float *a, unsigned long n) {
    float sum = 0.;
    for (unsigned long i = 0; i < n; ++i) {
        sum += a[i];
    }
    return sum;
}

float** matrix(size_t m, size_t n, sycl::queue &q) {
    float *a = sycl::malloc_shared<float>(m * n, q);
    float **b = sycl::malloc_shared<float*>(m, q);

    for (size_t i = 0; i < m; ++i) {
        b[i] = a + i * n;
    }
    return b;
}

float*** tensor3d(size_t l, size_t m, size_t n, sycl::queue &q) {
    float *a = sycl::malloc_shared<float>(l * m * n, q);
    float **b = sycl::malloc_shared<float*>(l * m, q);
    float ***c = sycl::malloc_shared<float**>(l, q);

    for (size_t i = 0; i < l; ++i) {
        c[i] = b + i * m;
        for (size_t j = 0; j < m; ++j) {
            c[i][j] = a + (i * m + j) * n;
        }
    }
    return c;
}

void free_matrix(float **matrix, sycl::queue &q) {
    sycl::free(matrix[0], q);
    sycl::free(matrix, q);
}

void free_tensor3d(float ***tensor3d, sycl::queue &q) {
    sycl::free(tensor3d[0][0], q);
    sycl::free(tensor3d[0], q);
    sycl::free(tensor3d, q);
}

float calculate_memory(Parameters *params) {
    return 1.e-9 * params->n_tallies * params->n_filter_bins * params->n_scores *
           params->n_nuclides * 3 * sizeof(float);
}
