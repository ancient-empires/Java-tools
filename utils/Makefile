src := $(shell find -name "*.c")
objs := $(src:.c=.o)

.PHONY: all
all: $(objs)

# common parts
common := ../common/Makefile
-include $(common)