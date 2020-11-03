TARGET := AE2lang

src := $(TARGET).c ../utils/utils.c
CFLAGS := -g -Wall -Werror

objs := $(src:.c=.o)

deps := $(src:.c=.d)
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)
CC_WRITE_DEP = $(CC) $(CFLAGS) -c $< -o "$@" $(DEPFLAGS)

.PHONY: all
all: $(TARGET).out

$(TARGET).out: $(objs)
	$(CC) $^ -o "$@"

%.o: %.c
	if [ "$(@D)" = . ]; then \
		$(CC_WRITE_DEP); \
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
