TARGET := AE2lang
src := $(TARGET).c ../utils/utils.c
objs := $(src:.c=.o)

CFLAGS := -g -Wall -Werror

.PHONY: all
all: $(TARGET).out

$(TARGET).out: $(objs)
	$(CC) $^ -o "$@"

%.o: %.c
	if [ "$(@D)" = . ]; then \
		$(CC) $(CFLAGS) -c "$^" -o $@; \
	else \
		$(MAKE) "$@" -C "$(@D)" CFLAGS="$(CFLAGS)"; \
	fi

.PHONY: clean
clean:
	-rm -rf *.o *.out
ifeq ($(R),1)
	for d in $(dir $(objs)); do \
		if [ "$$d" != ./ ]; then \
			$(MAKE) $@ -C $$d; \
		fi; \
	done
endif

# Check variables "DAT" and "TXT" before making conversions
ERROR_MSG := "ERROR: Invalid DAT and/or TXT file"
define CHECK_ARGS =
$(if $(DAT),,$(error $(ERROR_MSG)))
$(if $(TXT),,$(error $(ERROR_MSG)))
endef

.PHONY: dat2txt
dat2txt: all
	$(call CHECK_ARGS)
	./$(TARGET).out $(DAT) $(TXT)

.PHONY: txt2dat
txt2dat: all
	$(call CHECK_ARGS)
	./$(TARGET).out $(TXT) $(DAT)
