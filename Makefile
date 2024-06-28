# name of the program
PROGRAM = sycl.exe

# names of source files
CPPSOURCES = io.cpp utils.cpp parameters.cpp tally.cpp simulation.cpp main.cpp

# names of object files (including main.o)
OBJECTS = $(CPPSOURCES:.cpp=.o)

# Intel(R) DPC++/C++ Compiler options
CPPFLAGS = -fsycl

# Use the Intel oneAPI DPC++/C++ Compiler
CPP = icpx

# Compile all source files into object files
%.o: %.cpp
	$(CPP) $(CPPFLAGS) -Wall -g -O0 -c $<

# Link objects
$(PROGRAM): $(OBJECTS)
	icpx -fsycl *.o -o $(PROGRAM) -Wall -g

# OS detection and setting appropriate clean command
ifeq ($(OS),Windows_NT)
    RM = del /f
else
    RM = rm -f
endif

# clean
clean:
	$(RM) *.ilk *.pdb $(OBJECTS) $(PROGRAM)
