#include "io.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <iomanip> // Necessario per std::setw e std::setfill
#include "utils.hpp"

Io::Io()
{
}

void Io::print_parameters(Parameters *params) const
{
    print_border();
    print_center("INPUT SUMMARY", 79);
    print_border();
    std::cout << "RNG seed:                      " << params->seed << std::endl;
    std::cout << "Number of threads:             " << params->n_threads << std::endl;
    std::cout << "Number of particles:           ";
    print_fancy_int(params->n_particles);
    std::cout << "Number of tallies:             " << params->n_tallies << std::endl;
    std::cout << "Number of filter bins:         " << params->n_filter_bins << std::endl;
    std::cout << "Number of scores:              " << params->n_scores << std::endl;
    std::cout << "Number of nuclides:            " << params->n_nuclides << std::endl;
    std::cout << "Number of events per particle: " << params->n_events << std::endl;
    std::cout << "Estimated memory usage:        " << calculate_memory(params) << " GB" << std::endl;
    print_border();
}

void Io::read_input(int argc, char *argv[], Parameters *params)
{
    // Collect raw input
    for (int i = 1; i < argc; ++i)
    {
        char *arg = argv[i];

        // Error message
        char message[1024] = "Could not parse command line input '";
        strcat_s(message, sizeof(message), arg);
        strcat_s(message, sizeof(message), "'");

        if (strcmp(arg, "-r") == 0 || strcmp(arg, "--seed") == 0)
        {
            if (++i < argc)
            {
                params->seed = atoll(argv[i]);
            }
            else
            {
                print_error(message);
            }
        }
        else if (strcmp(arg, "-c") == 0 || strcmp(arg, "--threads") == 0)
        {
            if (++i < argc)
            {
                params->n_threads = atoi(argv[i]);
            }
            else
            {
                print_error(message);
            }
        }
        else if (strcmp(arg, "-p") == 0 || strcmp(arg, "--particles") == 0)
        {
            if (++i < argc)
            {
                params->n_particles = atoll(argv[i]);
            }
            else
            {
                print_error(message);
            }
        }
        else if (strcmp(arg, "-t") == 0 || strcmp(arg, "--tallies") == 0)
        {
            if (++i < argc)
            {
                params->n_tallies = atoi(argv[i]);
            }
            else
            {
                print_error(message);
            }
        }
        else if (strcmp(arg, "-f") == 0 || strcmp(arg, "--filter-bins") == 0)
        {
            if (++i < argc)
            {
                params->n_filter_bins = atoi(argv[i]);
            }
            else
            {
                print_error(message);
            }
        }
        else if (strcmp(arg, "-s") == 0 || strcmp(arg, "--scores") == 0)
        {
            if (++i < argc)
            {
                params->n_scores = atoi(argv[i]);
            }
            else
            {
                print_error(message);
            }
        }
        else if (strcmp(arg, "-n") == 0 || strcmp(arg, "--nuclides") == 0)
        {
            if (++i < argc)
            {
                params->n_nuclides = atoi(argv[i]);
            }
            else
            {
                print_error(message);
            }
        }
        else if (strcmp(arg, "-e") == 0 || strcmp(arg, "--events") == 0)
        {
            if (++i < argc)
            {
                params->n_events = std::stof(argv[i]);
            }
            else
            {
                print_error(message);
            }
        }
        else
        {
            print_error("Could not parse command line input");
        }
    }
}

void Io::print_error(const char *message) const
{
    std::cerr << "ERROR: " << message << std::endl;
    std::exit(1);
}

void Io::print_center(const char *s, int width) const
{
    for (int i = 0; i <= (width - std::strlen(s)) / 2; ++i)
    {
        std::cout << " ";
    }
    std::cout << s << std::endl;
}

void Io::print_fancy_int(long a) const
{
    if (a < 1000)
    {
        std::cout << a << std::endl;
    }
    else if (a >= 1000 && a < 1000000)
    {
        std::cout << a / 1000 << "," << std::setw(3) << std::setfill('0') << a % 1000 << std::endl;
    }
    else if (a >= 1000000 && a < 1000000000)
    {
        std::cout << a / 1000000 << "," << std::setw(3) << std::setfill('0') << (a % 1000000) / 1000 << "," << std::setw(3) << std::setfill('0') << a % 1000 << std::endl;
    }
    else if (a >= 1000000000)
    {
        std::cout << a / 1000000000 << "," << std::setw(3) << std::setfill('0') << (a % 1000000000) / 1000000 << "," << std::setw(3) << std::setfill('0') << (a % 1000000) / 1000 << "," << std::setw(3) << std::setfill('0') << a % 1000 << std::endl;
    }
    else
    {
        std::cout << a << std::endl;
    }
}

void Io::print_border() const
{
    std::cout << "=========================================="
                 "======================================"
              << std::endl;
}

void Io::showdevice(sycl::queue &q)
{
    Io::print_border();
    Io::print_center("DEVICE INFO", 79);
    Io::print_border();
    // Output platform and device information.
    auto device = q.get_device();
    auto p_name = device.get_platform().get_info<sycl::info::platform::name>();
    std::cout << std::setw(20) << "Platform Name: " << p_name << "\n";
    auto p_version = device.get_platform().get_info<sycl::info::platform::version>();
    std::cout << std::setw(20) << "Platform Version: " << p_version << "\n";
    auto d_name = device.get_info<sycl::info::device::name>();
    std::cout << std::setw(20) << "Device Name: " << d_name << "\n";
    auto max_work_group = device.get_info<sycl::info::device::max_work_group_size>();
    std::cout << std::setw(20) << "Max Work Group: " << max_work_group << "\n";
    auto max_compute_units = device.get_info<sycl::info::device::max_compute_units>();
    std::cout << std::setw(20) << "Max Compute Units: " << max_compute_units << "\n";
    std::cout << "Device Vendor: " << device.get_info<sycl::info::device::vendor>() << "\n";
    std::cout << "Driver Version: " << device.get_info<sycl::info::device::driver_version>() << "\n";
    std::cout << "FP64 Support: " << (device.has(sycl::aspect::fp64) ? "Yes" : "No") << "\n";
    Io::print_border();
}
