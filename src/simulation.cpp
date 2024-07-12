#include "simulation.hpp"
#include <sycl/sycl.hpp>
#include "random.hpp"
#include "tally.hpp"

#define weight 1.0f

unsigned long long simulate(Parameters *params, Tally *tallies, sycl::queue &q)
{
  try
  {
    unsigned long long verification = 0;

    sycl::buffer<unsigned long long, 1> verification_buf(&verification, 1);

    // Loop over particles
    q.submit([&](sycl::handler &h)
             { h.parallel_for(sycl::range<1>(params->n_workitem),
                              sycl::reduction(verification_buf,h, std::plus<unsigned long long>()),
                              [=](sycl::item<1> idx, auto &verification_reduction)
                              {
                                // Calculate start and end indices for particles
                                unsigned long work_size = (params->n_particles + params->n_workitem - 1) / params->n_workitem;
                                unsigned long start_idx = idx[0] * work_size;
                                unsigned long end_idx = sycl::min(start_idx + work_size, params->n_particles);

                                for (unsigned long i_p = start_idx; i_p < end_idx; ++i_p)
                                {
                                  // Get the RNG seed
                                  uint64_t seed = init_seed(i_p + 1, params->seed);

                                  float distance = rn(&seed);
                                  float flux = weight * distance;

                                  // Sample a number of tallying events (e.g. number of collisions or tracks)
                                  // for this particle
                                  int n_event = sample_n_events(params, &seed);

                                  // Loop over tallying events
                                  for (int i_e = 0; i_e < n_event; ++i_e)
                                  {
                                    // Sample a cell containing fuel material to score to by randomly
                                    // selecting a filter bin and tally index
                                    int filter_index = rni(0, params->n_filter_bins, &seed);
                                    int tally_index = rni(0, params->n_tallies, &seed);

                                    // Loop over tallies
                                    for (int i_t = 0; i_t < params->n_tallies; ++i_t)
                                    {
                                      // Search the levels of the nested geometry for the sampled cell
                                      int i_c;
                                      for (i_c = 0; i_c < params->n_coord; ++i_c)
                                      {
                                        if (i_c == tallies[i_t].filter_bins[filter_index])
                                        {
                                          break;
                                        }
                                      }

                                      // If this cell does not score to this tally, continue to the next
                                      // tally/filter; otherwise, score to this tally
                                      if (tally_index != i_t)
                                      {
                                        continue;
                                      }

                                      // Loop over nuclides. We are only scoring to tallies in fuel, so we
                                      // will always loop over all the nuclides, which represent the
                                      // nuclides in the fuel material.
                                      for (int i_n = 0; i_n < params->n_nuclides; ++i_n)
                                      {
                                        // Get the atomic density
                                        float atom_density = rn(&seed);

                                        // Get the microscopic cross section
                                        float micro_xs = rn(&seed);

                                        // Loop over scores
                                        for (int i_s = 0; i_s < params->n_scores; ++i_s)
                                        {
                                          // Determine the score index in the tally array
                                          int score_index = i_n * params->n_scores + i_s;

                                          // For tracklength tallies, the score is computed as
                                          // micro_xs.total * atom_density * flux, where flux =
                                          // particle_weight * distance
                                          float score = micro_xs * atom_density * flux;

                                          int tally_type = VALUE;
                                          increment_value_atomic(tallies[i_t].results, tallies[i_t].n_filter_bins, tallies[i_t].n_scores, filter_index, score_index, tally_type, score);

                                          // Update verification checksum
                                          unsigned long long *ptr = (unsigned long long *)&score;
                                          verification_reduction += *ptr;
                                        }
                                      }
                                      verification_reduction += (unsigned long long)i_c;
                                    }
                                  }
                                }
                              }); })
        .wait_and_throw();
       sycl::host_accessor v_out(verification_buf, sycl::read_only);
    return v_out[0];
  }
  catch (std::exception const &e)
  {
    std::cerr << "SYCL exception caught: " << e.what() << " in " << __FILE__ << " at line " << __LINE__ << std::endl;
  }
  catch (...)
  {
    std::cerr << "Unknown exception caught in " << __FILE__ << " at line " << __LINE__ << std::endl;
  }
  return NULL;
}

int sample_n_events(Parameters *params, uint64_t *seed)
{
  // Sample the number of events from a normal distribution with mean and
  // standard deviation determined by the tally estimator and the number of
  // radial regions in fuel
  float z;
  do
  {
    z = sample_gaussian(params->n_events, 0.5f * params->n_events, seed);
  } while (z < 0);
  int n = sycl::round(z);
  return n;
}
