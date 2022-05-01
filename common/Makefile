# This Makefile contains some common variables that can be used across various Makefiles.

# set compile flags
CFLAGS := -g -Wall -Werror
CXXFLAGS := -g -Wall -Werror

# set library flags
LDLIBS := -lstdc++ # use gcc to link to Standard C++ libraries

# automatic prerequisite generation
# source: http://web.archive.org/web/20210820223028/http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
DEPFLAGS = -MT "$@" -MMD -MP -MF "$(@:.o=.d)"
CC_WRITE_DEP = $(CC) $(CFLAGS) -c "$<" -o "$@" $(DEPFLAGS)
CXX_WRITE_DEP = $(CXX) $(CXXFLAGS) -c "$<" -o "$@" $(DEPFLAGS)

# in a Makefile that includes this Makefile,
# define the "objs" variable to specify all object files needed
deps = $(objs:.o=.d)
-include $(deps)

%.o: %.cpp
	$(CXX_WRITE_DEP)

%.o: %.c
	$(CC_WRITE_DEP)

# clean up
CLEAN := -rm -rfv *.o *.d *.out *.gch

.PHONY: clean
clean:
	$(CLEAN)
	rm -rfv $(objs) $(deps)
