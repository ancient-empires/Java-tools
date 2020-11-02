TARGET := utils

CFLAGS := -g -Wall -Werror

.PHONY: all
all: $(TARGET).o

deps := $(TARGET).d
-include $(deps)

%.o: %.c %.d
	$(CC) $(CFLAGS) -c $< -o "$@"

%.d: %.c
	$(CC) -MM $^ -MF $@

.PHONY: clean
clean:
	-rm -rf *.o *.d
