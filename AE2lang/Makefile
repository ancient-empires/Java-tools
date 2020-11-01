TARGET := AE2lang

.PHONY: all
all: $(TARGET).out

$(TARGET).out: $(TARGET).o
	$(CC) $^ -o "$@"

%.o: %.c
	$(CC) -g -Wall -Werror -c $^ -o "$@"

.PHONY: clean
clean:
	-rm -rf *.o *.out

.PHONY: dat2txt
dat2txt: all
	./$(TARGET).out lang.dat lang.txt

.PHONY: dat2txt
txt2dat: all
	./$(TARGET).out lang.txt lang.dat
