# This Makefile contains some common variables that can be used across various Makefiles.

# set compile flags
CFLAGS := -g -Wall -Werror
CXXFAGS := -g -Wall -Werror

# set linking flags (use gcc command to compile and link C++)
LDFLAGS := -lstdc++

# automatic prerequisite generation
DEPFLAGS = -MT "$@" -MMD -MP -MF "$(@:.o=.d)"
CC_WRITE_DEP = $(CC) $(CFLAGS) -c "$<" -o "$@" $(DEPFLAGS)
CXX_WRITE_DEP = $(CXX) $(CXXFLAGS) -c "$<" -o "$@" $(DEPFLAGS)

# clean up
CLEAN := -rm -rf *.o *.d *.out *.gch
