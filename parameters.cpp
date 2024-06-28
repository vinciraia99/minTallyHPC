#include "parameters.hpp"
#include "io.hpp"
#include <cstdlib>

Parameters *initialize_parameters(int argc, char *argv[])
{
  Parameters *params = static_cast<Parameters *>(std::malloc(sizeof(Parameters)));

  // Assegna parametri predefiniti
  params->seed = 12345;
  params->n_threads = 1; // Senza OpenMP, usiamo un solo thread per default
  params->n_particles = 100000;
  params->n_tallies = 180;
  params->n_filter_bins = 50952;
  params->n_scores = 6;
  params->n_nuclides = 6;
  params->n_coord = 10;

  // Il numero di eventi di tally per particella è il numero medio di
  // collisioni o tracce nel combustibile. I numeri predefiniti sono stati
  // stimati dai benchmark BEAVRS, ExaSMR e Monte Carlo e sono approssimativamente
  // gli stessi per ogni benchmark. Il numero medio di collisioni era 5.4 con
  // deviazione standard 3.2, e il numero medio di tracce era 20.7 con
  // deviazione standard 10.5. Il numero medio di tracce nel combustibile scala linearmente
  // con il numero di regioni radiali.
  params->n_events = 5.4;

  // Leggi gli argomenti della riga di comando
  Io io;
  io.read_input(argc, argv, params);

  return params;
}

void free_parameters(Parameters *params)
{
  std::free(params);
}
