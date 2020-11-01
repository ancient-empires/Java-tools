TARGET := AE2pak

.PHONY: all
all: $(TARGET).out

$(TARGET).out: $(TARGET).o
	$(CC) $^ -o "$@"

%.o: %.c
	$(CC) -g -Wall -Werror -c $^ -o "$@"

.PHONY: clean
clean:
	-rm -rf *.o *.out
