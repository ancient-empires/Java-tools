TARGET := utils

CFLAGS := -g -Wall -Werror

.PHONY: all
all: $(TARGET).o

deps := $(TARGET).d
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o "$@" $(DEPFLAGS)

.PHONY: clean
clean:
	-rm -rf *.o *.d
