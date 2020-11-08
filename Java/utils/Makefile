TARGET := utils

src := $(TARGET).c
objs := $(src:.c=.o)

.PHONY: all
all: $(TARGET).o

# common parts
common := ../common/Makefile
-include $(common)

# automatic prerequisite generation
deps := $(src:.c=.d)
-include $(deps)

%.o: %.c
	$(CC_WRITE_DEP)

.PHONY: clean
clean:
	$(CLEAN)
