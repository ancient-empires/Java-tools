TARGET := AE2pak

src := main.c ../utils/utils.c extract.c
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
