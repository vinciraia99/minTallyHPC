#include "tally.hpp"
#include "random.hpp"
#include "utils.hpp"
#include <sycl/sycl.hpp>

Tally* initialize_tallies(Parameters* params, sycl::queue& q) {
    // Get the RNG seed
    uint64_t seed = init_seed(0,params->seed);

    Tally* tallies = sycl::malloc_shared<Tally>(params->n_tallies, q);

    for (int i = 0; i < params->n_tallies; ++i) {
        tallies[i].n_scores = params->n_nuclides * params->n_scores;
        tallies[i].n_filter_bins = params->n_filter_bins;
        tallies[i].filter_bins = sycl::malloc_shared<int>(params->n_filter_bins, q);

        for (int j = 0; j < params->n_filter_bins; ++j) {
            tallies[i].filter_bins[j] = rni(0, params->n_coord, &seed);
        }

        // Allocate space for each filter bin and score combination and the three
        // types of tally results
        tallies[i].results = tensor3d(tallies[i].n_filter_bins, tallies[i].n_scores, 3, q);
    }
    return tallies;
}

void free_tallies(Tally* tallies, int n, sycl::queue& q) {
    for (int i = 0; i < n; ++i) {
        sycl::free(tallies[i].filter_bins, q);
        free_tensor3d(tallies[i].results, q);
    }
    sycl::free(tallies, q);
}
