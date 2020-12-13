target := AE2units.out

src_cpp := main.cpp UnitProcessor.cpp UnitInfo.cpp
src_c := ../utils/utils.c

objs = $(objs_c) $(objs_cpp)
objs_c := $(src_c:.c=.o)
objs_cpp := $(src_cpp:.cpp=.o)

.PHONY: all
all: $(target)

# common compile options
common := ../common/Makefile
-include $(common)

# automatic dependency tracking
deps = $(objs:.o=.d)
-include $(deps)

$(target): $(objs)
	$(CXX) $(CXXFLAGS) $^ -o "$@"

%.o: %.cpp
	$(CXX_WRITE_DEP)

%.o: %.c
	$(CC_WRITE_DEP)

.PHONY: clean
clean:
	-rm -rf *.o *.out
