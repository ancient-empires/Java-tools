# This Makefile contains some common variables that can be used across various Makefiles.

# set compile flags
CFLAGS := -g -Wall -Werror
CXXFLAGS := -g -Wall -Werror

# set linking flags (use gcc command to compile and link C++)
LDFLAGS := -lstdc++

# automatic prerequisite generation
# source: http://web.archive.org/web/20210820223028/http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
DEPFLAGS = -MT "$@" -MMD -MP -MF "$(@:.o=.d)"
CC_WRITE_DEP = $(CC) $(CFLAGS) -c "$<" -o "$@" $(DEPFLAGS)
CXX_WRITE_DEP = $(CXX) $(CXXFLAGS) -c "$<" -o "$@" $(DEPFLAGS)

# clean up
CLEAN := -rm -rf *.o *.d *.out *.gch
