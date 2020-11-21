# This Makefile contains some common variables that can be used across various Makefiles.

# set compile flags
CFLAGS := -g -Wall -Werror

# automatic prerequisite generation
DEPFLAGS = -MMD -MF $(@:.o=.d)
CC_WRITE_DEP = $(CC) $(CFLAGS) -c $< -o "$@" $(DEPFLAGS)

# clean up
CLEAN := -rm -rf *.o *.d *.out *.gch
