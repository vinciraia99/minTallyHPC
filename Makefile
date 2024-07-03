# name of the program
PROGRAM = sycl.exe

# directory for source files
SRCDIR = src

# directory for header files
INCDIR = include

# find all .cpp files in the src directory
CPPSOURCES = $(wildcard $(SRCDIR)/*.cpp)

# names of object files (including main.o)
OBJECTS = $(CPPSOURCES:.cpp=.o)

# Intel(R) DPC++/C++ Compiler options
CPPFLAGS = -fsycl -I$(INCDIR)

# Use the Intel oneAPI DPC++/C++ Compiler
CPP = icpx

# Compile all source files into object files
$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CPP) $(CPPFLAGS) -Wall -g -O0 -c $< -o $@

# Link objects
$(PROGRAM): $(OBJECTS)
	$(CPP) $(CPPFLAGS) $(OBJECTS) -o $(PROGRAM) -Wall -g

# OS detection and setting appropriate clean command
#ifeq ($(OS),Windows_NT)
#    RM = del /f
#else
    RM = rm -f
#endif

# clean
clean:
	$(RM) *.pdb *.ilk src/*.o $(PROGRAM)

cleanw:
	del /f *.pdb *.ilk src\*.o $(PROGRAM)
