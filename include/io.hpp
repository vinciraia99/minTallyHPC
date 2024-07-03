#ifndef IO_HPP
#define IO_HPP
#include <sycl/sycl.hpp>

#include "parameters.hpp"

class Io
{
public:
    Io();
    void read_input(int argc, char *argv[], Parameters *params);
    void print_parameters(Parameters *params) const;
    void print_border() const;
    void showdevice(sycl::queue &q);
private:
    void print_error(const char *message) const;
    void print_center(const char *s, int width) const;
    void print_fancy_int(long a) const;
};

#endif // IO_HPP
