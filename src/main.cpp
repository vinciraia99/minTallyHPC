#include <iostream>
#include "io.hpp"
#include "random.hpp"
#include "simulation.hpp"
#include "tally.hpp"
#include "utils.hpp"
#include "parameters.hpp"
#include <sycl/sycl.hpp>

using namespace std;

int main(int argc, char *argv[]) {
    Io io;
    sycl::queue q;
    io.showdevice(q);
    // Load the problem input parameters
    cout <<"Initializing problem inputs...\n"<< endl;
    Parameters *params = initialize_parameters(argc, argv,q);
    io.print_parameters(params);

    // Set up the tally structure
    cout <<"Initializing tallies..."<< endl;
    Tally *tallies = initialize_tallies(params,q);

    // Simulation loop
    cout << "Simulating tally events..."<< endl;
    double start = get_time();
    unsigned long long verification = simulate(params, tallies,q);
    double end = get_time();

    // Print results
    io.print_border();
    cout << "Verification checksum: " << verification << endl;
    cout << "Simulation time:" << end - start << " seconds"<< endl;

    // Free memory
    free_tallies(tallies, params->n_tallies,q);
    free_parameters(params,q);

    return 0;
}
