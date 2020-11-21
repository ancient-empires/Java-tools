TARGET := AE2pak

ALL := all
EXTRACT := extract
PACK := pack

src := main.c ../utils/utils.c path_processing.c file_processing.c extract.c pack.c
objs := $(src:.c=.o)

.PHONY: $(ALL)
$(ALL): $(TARGET).out

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

AE1data := data/AE1
AE1pak_pak := $(AE1data)/1.pak
AE1pak_dir := $(AE1data)/extracted
AE1pak_filelist := AE1_filelist.log

AE2data := data/AE2
AE2pak_pak := $(AE2data)/1.pak
AE2pak_dir := $(AE2data)/extracted
AE2pak_filelist := AE2_filelist.log

# check error for extraction
EXTRACT_ERROR_MSG := ERROR: Invalid .pak file and/or extract directory
define CHECK_EXTRACT_ERROR =
$(if $(pak),,$(error $(EXTRACT_ERROR_MSG)))
$(if $(dir),,$(error $(EXTRACT_ERROR_MSG)))
endef

.PHONY: check_extract_error
check_extract_error:
	$(call CHECK_EXTRACT_ERROR)

.PHONY: $(EXTRACT)
$(EXTRACT): $(ALL) check_extract_error
	mkdir -vp $(dir)
	./$(TARGET).out $(pak) -e $(dir)

.PHONY: $(EXTRACT)_all
$(EXTRACT)_all: $(EXTRACT)_ae1 $(EXTRACT)_ae2

.PHONY: $(EXTRACT)_ae1
$(EXTRACT)_ae1:
	$(MAKE) $(EXTRACT) pak=$(AE1pak_pak) dir=$(AE1pak_dir)

.PHONY: $(EXTRACT)_ae2
$(EXTRACT)_ae2:
	$(MAKE) $(EXTRACT) pak=$(AE2pak_pak) dir=$(AE2pak_dir)


# clean up extracted files

CLEAN_RES := clean

.PHONY: $(CLEAN_RES)_all_extracted
$(CLEAN_RES)_all_extracted: $(CLEAN_RES)_ae1 $(CLEAN_RES)_ae2

.PHONY: $(CLEAN_RES)_ae1
$(CLEAN_RES)_ae1:
	-rm -rfv $(AE1pak_dir) $(AE1_filelist)

.PHONY: $(CLEAN_RES)_ae2
$(CLEAN_RES)_ae2:
	-rm -rfv $(AE2pak_dir) $(AE2_filelist)


# pack .pak

# check error for extraction
PACK_ERROR_MSG := ERROR: Invalid .pak file and/or pack directory
define CHECK_PACK_ERROR =
$(if $(pak),,$(error $(PACK_ERROR_MSG)))
$(if $(dir),,$(error $(PACK_ERROR_MSG)))
endef

.PHONY: $(PACK)
$(PACK): $(ALL)
	$(call CHECK_PACK_ERROR)
# use a temporary file to hold the file list
	$(eval filelist := $(shell mktemp --suffix=.log))
	find $(dir)/* | sort >> $(filelist)
	-./$(TARGET).out $(pak) -p $(filelist)
	-rm -rfv $(filelist)
