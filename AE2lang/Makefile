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

# check error for DAT & TXT conversion
ERROR_MSG := ERROR: No valid DAT and / or TXT file specified
define CHECK_ARGS
ifndef DAT
$(error $(ERROR_MSG))
endif
ifndef TXT
$(error $(ERROR_MSG))
endif
endef

.PHONY: dat2txt
dat2txt: all
	$(call CHECK_ARGS)
	./$(TARGET).out $(DAT) $(TXT)

.PHONY: dat2txt
txt2dat: all
	$(call CHECK_ARGS)
	./$(TARGET).out $(TXT) $(DAT)
