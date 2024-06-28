#include <iostream>
#include <cstdio>
#include "io.hpp"
#include "parameters.hpp"
#include "simulation.hpp"
#include "tally.hpp"
#include "utils.hpp"

int main(int argc, char *argv[])
{
  std::cout << "Initializing problem inputs..." << std::endl;
  Parameters *params = initialize_parameters(argc, argv);
  Io io;
  io.read_input(argc, argv, params);
  io.print_parameters(params);
  // print_parameters(params);

  Random::set_seed(params->seed);

  std::cout << "Initializing tallies..." << std::endl;
  Tally *tallies = initialize_tallies(params);

  std::cout << "Simulating tally events..." << std::endl;
  double start = get_time();
  unsigned long long verification = simulate(params, tallies, Random::get_master_seed());
  double end = get_time();

  // print_border();
  std::cout << "Verification checksum: " << verification << std::endl;
  std::cout << "Simulation time:       " << (end - start) << " seconds" << std::endl;

  free_tallies(tallies, params->n_tallies);
  free_parameters(params);

  return 0;
}
