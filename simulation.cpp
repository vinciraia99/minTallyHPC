#include <sycl/sycl.hpp>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include "parameters.hpp"
#include "simulation.hpp"
#include "tally.hpp"

using namespace sycl;

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

SYCL_EXTERNAL float rn(uint64_t *seed)
{
  *seed = (mult * (*seed) + inc) & mask;
  return (*seed) / mod;
}

SYCL_EXTERNAL int rni(int a, int b, uint64_t *seed)
{
  *seed = (mult * (*seed) + inc) & mask;
  return a + (b * (*seed) / (mod + a));
}

int64_t Random::master_seed = 1;

float sample_gaussian(float mu, float sigma, uint64_t *seed)
{
  float theta = 2 * M_PI * rn(seed);
  float r = sqrt(-2 * log(rn(seed)));
  return r * cos(theta) * sigma + mu;
}

SYCL_EXTERNAL int sample_n_events(const Parameters *params, uint64_t *seed)
{
  float z;
  do
  {
    z = sample_gaussian(params->n_events, 0.5f * params->n_events, seed);
  } while (z < 0);
  return round(z);
}

/*Tally *initialize_tallies(Parameters *params)
{
  Tally *tallies = (Tally *)malloc(sizeof(Tally) * params->n_tallies);

  for (int i = 0; i < params->n_tallies; ++i)
  {
    tallies[i].n_scores = params->n_scores;
    tallies[i].n_filter_bins = params->n_filter_bins;
    tallies[i].filter_bins = (int *)malloc(sizeof(int) * params->n_filter_bins);

    tallies[i].results = (double ***)malloc(sizeof(double **) * params->n_filter_bins);
    for (int j = 0; j < params->n_filter_bins; ++j)
    {
      tallies[i].results[j] = (double **)malloc(sizeof(double *) * params->n_scores);
      for (int k = 0; k < params->n_scores; ++k)
      {
        tallies[i].results[j][k] = (double *)malloc(sizeof(double) * 3);
        tallies[i].results[j][k][VALUE] = 0.0;
        tallies[i].results[j][k][SUM] = 0.0;
        tallies[i].results[j][k][SUM_SQ] = 0.0;
      }
    }
  }
  return tallies;
}

void free_tallies(Tally *tallies, int n)
{
  for (int i = 0; i < n; ++i)
  {
    for (int j = 0; j < tallies[i].n_filter_bins; ++j)
    {
      for (int k = 0; k < tallies[i].n_scores; ++k)
      {
        free(tallies[i].results[j][k]);
      }
      free(tallies[i].results[j]);
    }
    free(tallies[i].results);
    free(tallies[i].filter_bins);
  }
  free(tallies);
}*/

/*unsigned long long simulate(Parameters *params, Tally *tallies, uint64_t master_seed)
{
  unsigned long long verification = 0;

  queue q;

  buffer<unsigned long long, 1> verification_buf(&verification, range<1>(1));
  buffer<Tally, 1> tallies_buf(tallies, range<1>(params->n_tallies));
  buffer<Parameters, 1> params_buf(params, range<1>(1));

  q.submit([&](handler &h)
           {
        auto verification_acc = verification_buf.get_access<access::mode::read_write>(h);
        auto tallies_acc = tallies_buf.get_access<access::mode::read_write>(h);
        auto params_acc = params_buf.get_access<access::mode::read>(h);

        h.parallel_for(range<1>(params->n_particles), [=](id<1> i_p) {
            uint64_t seed = Random::future_seed(i_p[0] + 1, master_seed); //sposta il future seed esterno

            const double weight = 1.0;
            double distance = rn(&seed);
            double flux = weight * distance;

            int n_event = sample_n_events(&params_acc[0], &seed);

            for (int i_e = 0; i_e < n_event; ++i_e)
            {
                int filter_index = rni(0, params_acc[0].n_filter_bins, &seed);
                int tally_index = rni(0, params_acc[0].n_tallies, &seed);

                for (int i_t = 0; i_t < params_acc[0].n_tallies; ++i_t)
                {
                    int i_c;
                    for (i_c = 0; i_c < params_acc[0].n_coord; ++i_c)
                    {
                        if (i_c == tallies_acc[i_t].filter_bins[filter_index])
                        {
                            break;
                        }
                    }

                    if (tally_index != i_t)
                    {
                        continue;
                    }

                    for (int i_n = 0; i_n < params_acc[0].n_nuclides; ++i_n)
                    {
                        double atom_density = rn(&seed);
                        double micro_xs = rn(&seed);

                        for (int i_s = 0; i_s < params_acc[0].n_scores; ++i_s)
                        {
                            int score_index = i_n * params_acc[0].n_scores + i_s;
                            double score = micro_xs * atom_density * flux;

                            atomic_ref<double, memory_order::relaxed, memory_scope::device, access::address_space::global_space> tally_atomic(tallies_acc[i_t].results[filter_index][score_index][VALUE]);
                            tally_atomic += score;

                            unsigned long long *ptr = reinterpret_cast<unsigned long long *>(&score);
                            atomic_ref<unsigned long long, memory_order::relaxed, memory_scope::device, access::address_space::global_space> verification_atomic(verification_acc[0]);
                            verification_atomic += *ptr;
                        }
                    }
                    atomic_ref<unsigned long long, memory_order::relaxed, memory_scope::device, access::address_space::global_space> verification_atomic(verification_acc[0]);
                    verification_atomic += i_c;
                }
            }
        }); })
      .wait();

  verification = verification_buf.get_host_access()[0];
  return verification;
}*/

/*unsigned long long simulate(Parameters *params, Tally *tallies, uint64_t master_seed)
{
  unsigned long long verification = 0;

  try
  {
    queue q;

    // Stampa informazioni sul dispositivo utilizzato
    auto device = q.get_device();
    std::cout << "Device Name: " << device.get_info<info::device::name>() << "\n";
    std::cout << "Device Vendor: " << device.get_info<info::device::vendor>() << "\n";
    std::cout << "Driver Version: " << device.get_info<info::device::driver_version>() << "\n";
    std::cout << "FP64 Support: " << (device.has(aspect::fp64) ? "Yes" : "No") << "\n";

    buffer<unsigned long long, 1> verification_buf(&verification, range<1>(1));
    buffer<Tally, 1> tallies_buf(tallies, range<1>(params->n_tallies));
    buffer<Parameters, 1> params_buf(params, range<1>(1));

    q.submit([&](handler &h)
             {
            auto verification_acc = verification_buf.get_access<access::mode::read_write>(h);
            auto tallies_acc = tallies_buf.get_access<access::mode::read_write>(h);
            auto params_acc = params_buf.get_access<access::mode::read>(h);

            h.parallel_for(range<1>(params->n_particles), [=](id<1> i_p) {
                uint64_t seed = Random::future_seed(i_p[0] + 1, master_seed); // sposta il future seed esterno

                const double weight = 1.0;
                double distance = rn(&seed);
                double flux = weight * distance;

                int n_event = sample_n_events(&params_acc[0], &seed);

                for (int i_e = 0; i_e < n_event; ++i_e)
                {
                    int filter_index = rni(0, params_acc[0].n_filter_bins, &seed);
                    int tally_index = rni(0, params_acc[0].n_tallies, &seed);

                    for (int i_t = 0; i_t < params_acc[0].n_tallies; ++i_t)
                    {
                        int i_c;
                        for (i_c = 0; i_c < params_acc[0].n_coord; ++i_c)
                        {
                            if (i_c == tallies_acc[i_t].filter_bins[filter_index])
                            {
                                break;
                            }
                        }

                        if (tally_index != i_t)
                        {
                            continue;
                        }

                        for (int i_n = 0; i_n < params_acc[0].n_nuclides; ++i_n)
                        {
                            double atom_density = rn(&seed);
                            double micro_xs = rn(&seed);

                            for (int i_s = 0; i_s < params_acc[0].n_scores; ++i_s)
                            {
                                int score_index = i_n * params_acc[0].n_scores + i_s;
                                double score = micro_xs * atom_density * flux;

                                atomic_ref<double, memory_order::relaxed, memory_scope::device, access::address_space::global_space> tally_atomic(tallies_acc[i_t].results[filter_index][score_index][VALUE]);
                                tally_atomic += score;

                                unsigned long long *ptr = reinterpret_cast<unsigned long long *>(&score);
                                atomic_ref<unsigned long long, memory_order::relaxed, memory_scope::device, access::address_space::global_space> verification_atomic(verification_acc[0]);
                                verification_atomic += *ptr;
                            }
                        }
                        atomic_ref<unsigned long long, memory_order::relaxed, memory_scope::device, access::address_space::global_space> verification_atomic(verification_acc[0]);
                        verification_atomic += i_c;
                    }
                }
            }); })
        .wait();

    verification = verification_buf.get_host_access()[0];
  }
  catch (exception const &e)
  {
    std::cerr << "SYCL exception caught: " << e.what() << std::endl;
  }
  catch (std::exception const &e)
  {
    std::cerr << "Standard exception caught: " << e.what() << std::endl;
  }
  catch (...)
  {
    std::cerr << "Unknown exception caught" << std::endl;
  }

  return verification;
}*/

float simulate(Parameters *params, Tally *tallies, uint64_t master_seed)
{
  float verification = 0;

  try
  {
    queue q;

    // Stampa informazioni sul dispositivo utilizzato
    auto device = q.get_device();
    std::cout << "Device Name: " << device.get_info<info::device::name>() << "\n";
    std::cout << "Device Vendor: " << device.get_info<info::device::vendor>() << "\n";
    std::cout << "Driver Version: " << device.get_info<info::device::driver_version>() << "\n";
    std::cout << "FP64 Support: " << (device.has(aspect::fp64) ? "Yes" : "No") << "\n";

    // Creazione dell'array di seed in parallelo utilizzando SYCL
    std::vector<uint64_t> seeds(params->n_particles);
    {
      buffer<uint64_t, 1> seeds_buf(seeds.data(), range<1>(params->n_particles));

      q.submit([&](handler &h)
               {
                auto seeds_acc = seeds_buf.get_access<access::mode::write>(h);

                h.parallel_for(range<1>(params->n_particles), [=](id<1> i_p) {
                    seeds_acc[i_p[0]] = Random::future_seed(i_p[0] + 1, master_seed);
                }); })
          .wait();
    }

    // Ordinamento dei seed in ordine decrescente sulla CPU
    std::sort(seeds.begin(), seeds.end(), std::greater<uint64_t>());

    std::cout << "Seed generati" << std::endl;

    // Copia dei seed ordinati sul dispositivo
    buffer<uint64_t, 1> seeds_buf(seeds.data(), range<1>(params->n_particles));

    buffer<float, 1> verification_buf(&verification, range<1>(1));
    buffer<Tally, 1> tallies_buf(tallies, range<1>(params->n_tallies));
    buffer<Parameters, 1> params_buf(params, range<1>(1));

    q.submit([&](handler &h)
             {
            auto verification_acc = verification_buf.get_access<access::mode::read_write>(h);
            auto tallies_acc = tallies_buf.get_access<access::mode::read_write>(h);
            auto params_acc = params_buf.get_access<access::mode::read>(h);
            auto seeds_acc = seeds_buf.get_access<access::mode::read>(h);

            h.parallel_for(range<1>(params->n_particles), [=](id<1> i_p) {
                uint64_t seed = seeds_acc[i_p[0]];

                const float weight = 1.0f; 
                float distance = rn(&seed);
                float flux = weight * distance;

                int n_event = sample_n_events(&params_acc[0], &seed);

                for (int i_e = 0; i_e < n_event; ++i_e)
                {
                    int filter_index = rni(0, params_acc[0].n_filter_bins, &seed);
                    int tally_index = rni(0, params_acc[0].n_tallies, &seed);
                    atomic_ref<float, memory_order::relaxed, memory_scope::device, access::address_space::global_space> verification_atomic(verification_acc[0]);
                    verification_atomic += 1;

                    /*for (int i_t = 0; i_t < params_acc[0].n_tallies; ++i_t)
                    {
                        int i_c;
                        for (i_c = 0; i_c < params_acc[0].n_coord; ++i_c)
                        {
                            if (i_c == tallies_acc[i_t].filter_bins[filter_index])
                            {
                                break;
                            }
                        

                        if (tally_index != i_t)
                        {
                            continue;
                        }

                        for (int i_n = 0; i_n < params_acc[0].n_nuclides; ++i_n)
                        {
                            float atom_density = rn(&seed);
                            float micro_xs = rn(&seed);

                            for (int i_s = 0; i_s < params_acc[0].n_scores; ++i_s)
                            {
                                int score_index = i_n * params_acc[0].n_scores + i_s;
                                float score = micro_xs * atom_density * flux;

                                atomic_ref<float, memory_order::relaxed, memory_scope::device, access::address_space::global_space> tally_atomic(tallies_acc[i_t].results[filter_index][score_index][VALUE]);
                                tally_atomic += score;

                                float *ptr = reinterpret_cast<float *>(&score);
                                atomic_ref<float, memory_order::relaxed, memory_scope::device, access::address_space::global_space> verification_atomic(verification_acc[0]);
                                verification_atomic += score;
                            }
                        }
                        atomic_ref<float, memory_order::relaxed, memory_scope::device, access::address_space::global_space> verification_atomic(verification_acc[0]);
                        verification_atomic += i_c;
                        }
                    }*/
                }
            }); })
        .wait_and_throw();

    verification = verification_buf.get_host_access()[0];
  }
  catch (exception const &e)
  {
    std::cerr << "SYCL exception caught: " << e.what() << " in " << __FILE__ << " at line " << __LINE__ << std::endl;
  }
  catch (std::exception const &e)
  {
    std::cerr << "Standard exception caught: " << e.what() << " in " << __FILE__ << " at line " << __LINE__ << std::endl;
  }
  catch (...)
  {
    std::cerr << "Unknown exception caught in " << __FILE__ << " at line " << __LINE__ << std::endl;
  }

  return verification;
}