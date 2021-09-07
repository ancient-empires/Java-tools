# Compile and link

TARGET := AE2lang

src := main.c ../utils/utils.c dat2txt.c txt2dat.c
objs := $(src:.c=.o)

.PHONY: all
all: $(TARGET).out

# executable file
$(TARGET).out: $(objs)
	$(CC) $^ -o "$@"

# common compile options
common := ../common/Makefile
-include $(common)

# automatic prerequisite generation
deps := $(src:.c=.d)
-include $(deps)

%.o: %.c
	$(CC_WRITE_DEP)

.PHONY: clean
clean:
	$(CLEAN)
	rm -rfv $(objs) $(deps)


# Conversions

AE1lang_dir := data/AE1
AE1lang_dat := $(AE1lang_dir)/lang.dat
AE1lang_txt := $(AE1lang_dir)/lang.txt

AE2lang_dir := data/AE2
AE2lang_dat := $(AE2lang_dir)/lang.dat
AE2lang_txt := $(AE2lang_dir)/lang.txt

# Check variables "dat" and "txt" before making conversions
ERROR_MSG := ERROR: Invalid .dat and/or .txt file
define CHECK_ARGS =
$(if $(dat),,$(error $(ERROR_MSG)))
$(if $(txt),,$(error $(ERROR_MSG)))
endef

# .dat to .txt

.PHONY: dat2txt
dat2txt: all
	$(call CHECK_ARGS)
	./$(TARGET).out $(dat) $(txt)

.PHONY: dat2txt_all
dat2txt_all: dat2txt_ae1 dat2txt_ae2

.PHONY: dat2txt_ae1
dat2txt_ae1:
	$(MAKE) dat2txt dat=$(AE1lang_dat) txt=$(AE1lang_txt)

.PHONY: dat2txt_ae2
dat2txt_ae2:
	$(MAKE) dat2txt dat=$(AE2lang_dat) txt=$(AE2lang_txt)


# .txt to .dat

.PHONY: txt2dat
txt2dat: all
	$(call CHECK_ARGS)
	./$(TARGET).out $(txt) $(dat)

.PHONY: txt2dat_all
txt2dat_all: txt2dat_ae1 txt2dat_ae2

.PHONY: txt2dat_ae1
txt2dat_ae1:
	$(MAKE) txt2dat dat=$(AE1lang_dat) txt=$(AE1lang_txt)

.PHONY: txt2dat_ae2
txt2dat_ae2:
	$(MAKE) txt2dat dat=$(AE2lang_dat) txt=$(AE2lang_txt)
