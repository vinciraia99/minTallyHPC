#ifndef IO_HPP
#define IO_HPP

#include "utils.hpp"

class Io
{
public:
    Io();
    void read_input(int argc, char *argv[], Parameters *params);
    void print_parameters(const Parameters *params) const;

private:
    void print_error(const char *message) const;
    void print_center(const char *s, int width) const;
    void print_fancy_int(long a) const;
    void print_border() const;
};

#endif // IO_HPP
