TARGET := utils

src := $(TARGET).c
objs := $(src:.c=.o)

CFLAGS := -g -Wall -Werror

.PHONY: all
all: $(TARGET).o

# common parts
-include ../common/Makefile

# automatic prerequisite generation
deps := $(src:.c=.d)
-include $(deps)

%.o: %.c
	$(CC_WRITE_DEP)

.PHONY: clean
clean:
	$(CLEAN)
