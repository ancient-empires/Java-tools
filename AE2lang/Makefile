target := AE2lang

.PHONY: all
all: $(target).out

$(target).out: $(target).o
	$(CC) $^ -o "$@"

%.o: %.c
	$(CC) -g -Wall -Werror -c $^ -o "$@"

.PHONY: clean
clean:
	-rm -rf *.o *.out

.PHONY: dat2txt
dat2txt: all
	./$(target).out lang.dat lang.txt

.PHONY: dat2txt
txt2dat: all
	./$(target).out lang.txt lang.dat
