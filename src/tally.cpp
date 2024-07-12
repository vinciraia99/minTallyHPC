#include "tally.hpp"
#include "random.hpp"
#include "utils.hpp"
#include <sycl/sycl.hpp>

Tally* initialize_tallies(Parameters* params, sycl::queue& q) {
    // Ottieni il seed RNG
    uint64_t seed = init_seed(0, params->seed);

    // Alloca memoria per i tally
    Tally* tallies = sycl::malloc_shared<Tally>(params->n_tallies, q);

    for (int i = 0; i < params->n_tallies; ++i) {
        tallies[i].n_scores = params->n_nuclides * params->n_scores;
        tallies[i].n_filter_bins = params->n_filter_bins;
        tallies[i].filter_bins = sycl::malloc_shared<int>(params->n_filter_bins, q);

        for (int j = 0; j < params->n_filter_bins; ++j) {
            tallies[i].filter_bins[j] = rni(0, params->n_coord, &seed);
        }

        // Alloca memoria per l'array monodimensionale dei risultati
        int results_size = tallies[i].n_filter_bins * tallies[i].n_scores * 3;
        tallies[i].results = sycl::malloc_shared<float>(results_size, q);

        // Inizializza l'array a zero
        for (int j = 0; j < results_size; ++j) {
            tallies[i].results[j] = 0.0f;
        }
    }
    return tallies;
}

void free_tallies(Tally* tallies, int n, sycl::queue& q) {
    for (int i = 0; i < n; ++i) {
        sycl::free(tallies[i].filter_bins, q);
        sycl::free(tallies[i].results, q);
    }
    sycl::free(tallies, q);
}

void set_value(float* array, int n_filter_bins, int n_scores, int filter_bin, int score, int tally_type, float value) {
    int idx = (filter_bin * n_scores * 3) + (score * 3) + tally_type;
    array[idx] = value;
}

float get_value(float* array, int n_filter_bins, int n_scores, int filter_bin, int score, int tally_type) {
    int idx = (filter_bin * n_scores * 3) + (score * 3) + tally_type;
    return array[idx];
}

void increment_value_atomic(float* array, int n_filter_bins, int n_scores, int filter_bin, int score, int tally_type, float increment) {
    int idx = (filter_bin * n_scores * 3) + (score * 3) + tally_type;
    auto atomic_ref = sycl::atomic_ref<float, sycl::memory_order::relaxed, sycl::memory_scope::device, sycl::access::address_space::global_space>(array[idx]);
    atomic_ref.fetch_add(increment);
}

