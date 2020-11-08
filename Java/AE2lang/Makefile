TARGET := AE2lang

src := $(TARGET).c ../utils/utils.c
objs := $(src:.c=.o)

.PHONY: all
all: $(TARGET).out

# common parts
common := ../common/Makefile
-include $(common)

$(TARGET).out: $(objs)
	$(CC) $^ -o "$@"

# automatic prerequisite generation
deps := $(src:.c=.d)
-include $(deps)

%.o: %.c
	if [ "$(@D)" = . ]; then \
		$(CC_WRITE_DEP); \
	else \
		# Generate targets in other directories \
		$(MAKE) "$@" -C "$(@D)" CFLAGS="$(CFLAGS)"; \
	fi

.PHONY: clean
clean:
	$(CLEAN)
# Use R=1 to recursively clean up generated files in other directories
ifeq ($(R),1)
	for d in $(dir $(objs)); do \
		if [ "$$d" != ./ ]; then \
			$(MAKE) "$@" -C "$$d"; \
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
