#include "utils.hpp"
#include <cstdlib>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

double get_time()
{
#ifdef _WIN32
  FILETIME ft;
  LARGE_INTEGER li;
  GetSystemTimeAsFileTime(&ft);
  li.LowPart = ft.dwLowDateTime;
  li.HighPart = ft.dwHighDateTime;
  return li.QuadPart * 1e-7;
#else
  struct timeval time;
  gettimeofday(&time, nullptr);
  return time.tv_sec + time.tv_usec / 1000000.0;
#endif
}

float sum(float *a, std::size_t n)
{
  float total = 0.0;
  for (std::size_t i = 0; i < n; ++i)
  {
    total += a[i];
  }
  return total;
}

float **matrix(std::size_t m, std::size_t n)
{
  float *a = static_cast<float *>(std::calloc(m * n, sizeof(float)));
  float **b = static_cast<float **>(std::malloc(m * sizeof(float *)));

  for (std::size_t i = 0; i < m; ++i)
  {
    b[i] = a + i * n;
  }
  return b;
}

float ***tensor3d(std::size_t l, std::size_t m, std::size_t n)
{
  float *a = static_cast<float *>(std::calloc(l * m * n, sizeof(float)));
  float **b = static_cast<float **>(std::malloc(l * m * sizeof(float *)));
  float ***c = static_cast<float ***>(std::malloc(l * sizeof(float **)));

  for (std::size_t i = 0; i < l; ++i)
  {
    c[i] = b + i * m;
    for (std::size_t j = 0; j < m; ++j)
    {
      c[i][j] = a + (i * m * n) + (j * n);
    }
  }
  return c;
}

void free_matrix(float **matrix)
{
  std::free(matrix[0]);
  std::free(matrix);
}

void free_tensor3d(float ***tensor3d)
{
  std::free(tensor3d[0][0]);
  std::free(tensor3d[0]);
  std::free(tensor3d);
}

float calculate_memory(const Parameters *params)
{
  return 1.e-9 * params->n_tallies * params->n_filter_bins * params->n_scores *
         params->n_nuclides * 3 * sizeof(float);
}
