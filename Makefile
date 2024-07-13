# Detect the operating system
ifeq ($(OS),Windows_NT)
    PROGRAM = mintally.exe
else
    PROGRAM = mintally.out
endif

# directory for source files
SRCDIR = src

# directory for header files
INCDIR = include

# find all .cpp files in the src directory
CPPSOURCES = $(wildcard $(SRCDIR)/*.cpp)

# names of object files (including main.o)
OBJECTS = $(CPPSOURCES:$(SRCDIR)/%.cpp=$(SRCDIR)/%.o)

# Intel(R) DPC++/C++ Compiler options
CPPFLAGS = -fsycl -I$(INCDIR) -O3 -Wall

# Use the Intel oneAPI DPC++/C++ Compiler
CPP = icpx

# Compile all source files into object files
$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

# Link objects
$(PROGRAM): $(OBJECTS)
	$(CPP) $(CPPFLAGS) $(OBJECTS) -o $(PROGRAM)

# clean
clean:
	 rm -f *.pdb *.ilk $(SRCDIR)/*.o $(PROGRAM)

cleanw:
	del /f *.pdb *.ilk $(SRCDIR)\*.o $(PROGRAM)
