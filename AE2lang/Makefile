TARGET := AE2lang

src := main.c ../utils/utils.c dat2txt.c txt2dat.c
objs := $(src:.c=.o)

.PHONY: all
all: $(TARGET).out

# common parts
common := ../common/Makefile
-include $(common)

# executable file
$(TARGET).out: $(objs)
	$(CC) $^ -o "$@"

# automatic prerequisite generation
deps := $(src:.c=.d)
-include $(deps)

%.o: %.c
	$(CC_WRITE_DEP)

.PHONY: clean
clean:
	$(CLEAN)
# Use R=1 to remove generated object files in other directories as well
ifeq ($(R),1)
	rm -rfv $(objs)
endif

# Check variables "dat" and "txt" before making conversions
ERROR_MSG := ERROR: Invalid .dat and/or .txt file
define CHECK_ARGS =
$(if $(dat),,$(error $(ERROR_MSG)))
$(if $(txt),,$(error $(ERROR_MSG)))
endef

.PHONY: dat2txt
dat2txt: all
	$(call CHECK_ARGS)
	./$(TARGET).out $(dat) $(txt)

.PHONY: txt2dat
txt2dat: all
	$(call CHECK_ARGS)
	./$(TARGET).out $(txt) $(dat)
