TARGET := AE2pak

src := main.c ../utils/utils.c path_processing.c file_processing.c extract.c pack.c
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

# extract .pak

EXTRACT := extract

AE1data := data/AE1/
AE1pak_pak := $(AE1data)/1.pak
AE1pak_dir := $(AE1data)/1/
AE1pak_filelist := AE1_filelist.log

AE2data := data/AE2/
AE2pak_pak := $(AE2data)/1.pak
AE2pak_dir := $(AE2data)/1/
AE2pak_filelist := AE2_filelist.log

# check error for extraction
EXTRACT_ERROR_MSG := ERROR: Invalid .pak file and/or extract directory and/or filelist .log file
define CHECK_EXTRACT_ERROR =
$(if $(pak),,$(error $(EXTRACT_ERROR_MSG)))
$(if $(dir),,$(error $(EXTRACT_ERROR_MSG)))
$(if $(filelist),,$(error $(EXTRACT_ERROR_MSG)))
endef

.PHONY: check_extract_error
check_extract_error:
	$(call CHECK_EXTRACT_ERROR)

.PHONY: $(EXTRACT)
$(EXTRACT): all check_extract_error
	mkdir -vp $(dir)
	./$(TARGET).out $(pak) -e $(dir) $(filelist)

.PHONY: extract_ae1
extract_ae1:
	$(MAKE) $(EXTRACT) pak=$(AE1pak_pak) dir=$(AE1pak_dir) filelist=$(AE1pak_filelist)

.PHONY: extract_ae2
extract_ae2:
	$(MAKE) $(EXTRACT) pak=$(AE2pak_pak) dir=$(AE2pak_dir) filelist=$(AE2pak_filelist)

# pack .pak

# check error for extraction
PACK_ERROR_MSG := ERROR: Invalid .pak file and/or file list .log
define CHECK_PACK_ERROR =
$(if $(pak),,$(error $(PACK_ERROR_MSG)))
$(if $(filelist),,$(error $(PACK_ERROR_MSG)))
endef

.PHONY: pack
pack: all
	$(call CHECK_PACK_ERROR)
	./$(TARGET).out $(pak) -p $(filelist)
