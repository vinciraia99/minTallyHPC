#include "parameters.hpp"
#include "io.hpp"
#include <sycl/sycl.hpp>

Parameters *initialize_parameters(int argc, char *argv[], sycl::queue &q)
{
  Parameters *params = sycl::malloc_shared<Parameters>(sizeof(Parameters), q);

  // Assign default parameters
  params->seed = 12345;
  params->n_particles = 1000;
  params->n_tallies = 180;
  params->n_filter_bins = 50952;
  params->n_scores = 6;
  params->n_nuclides = 6;
  params->n_coord = 10;

  // The number of tally events per particle is the average number of
  // collisions or track in fuel. The default numbers were estimated from the
  // BEAVRS, ExaSMR, and Monte Carlo performance benchmarks and are approxmatly
  // the same for each benchmark. The mean number of collisions was 5.4 with
  // standard deviation 3.2, and the mean number of tracks was 20.7 with
  // standard deviation 10.5. The mean number of track in fuel scales linearly
  // with the number of radial regions.
  params->n_events = 5.4f;

  sycl::device device = q.get_device();
  size_t max_work_group_size = device.get_info<sycl::info::device::max_work_group_size>();

  params->n_workitem = max_work_group_size;

  // Read command line arguments
  Io io;
  io.read_input(argc, argv, params);

  return params;
}

void free_parameters(Parameters *params, sycl::queue &q)
{
  sycl::free(params, q);
}
