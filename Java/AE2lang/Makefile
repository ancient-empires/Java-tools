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

ERROR_MSG := "ERROR: Invalid DAT and/or TXT file"
define CHECK_ARGS =
$(if $(DAT),,$(error $(ERROR_MSG)))
$(if $(TXT),,$(error $(ERROR_MSG)))
endef

.PHONY: dat2txt
dat2txt: all
	$(call CHECK_ARGS)
	./$(TARGET).out $(DAT) $(TXT)

.PHONY: dat2txt
txt2dat: all
	$(call CHECK_ARGS)
	./$(TARGET).out $(TXT) $(DAT)
