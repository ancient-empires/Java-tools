TARGET := AE2units.out

src_cpp := $(shell find -name "*.cpp")
src_c := ../utils/utils.c

objs = $(objs_c) $(objs_cpp)
objs_c := $(src_c:.c=.o)
objs_cpp := $(src_cpp:.cpp=.o)

.PHONY: all
all: $(TARGET)

# all executables
$(TARGET): $(objs)
	$(CXX) $(CXXFLAGS) $^ -o $@

# common compile options
common := ../common/Makefile
-include $(common)

# data files
data := data
units_bin := $(data)/units.bin
unit_files := $(shell find $(data) -name "*.unit")

.PHONY: extract
extract: $(TARGET)
	./$< -e $(units_bin) $(data)

.PHONY: pack
pack: $(TARGET)
ifdef unit_files
	./$< -p $(units_bin) $(data)
else
	$(error ERROR: no files to pack. Try to run `make extract` to extract the *.unit files first)
endif

.PHONY: clean_all_extracted
clean_all_extracted:
	-rm -rfv $(data)/*.unit