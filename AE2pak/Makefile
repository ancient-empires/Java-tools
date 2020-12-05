# Compile and link

TARGET := AE2pak

src := main.c ../utils/utils.c path_processing.c file_processing.c extract.c pack.c
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
	rm -rfv $(objs) $(deps)


# Extract .pak

AE1data := data/AE1
AE1pak_pak := $(AE1data)/1.pak
AE1pak_dir := $(AE1data)/extracted
AE1pak_filelist := $(AE1data)/filelist.log

AE2data := data/AE2
AE2pak_pak := $(AE2data)/1.pak
AE2pak_dir := $(AE2data)/extracted
AE2pak_filelist := $(AE2data)/filelist.log

# check error for extraction
EXTRACT_ERROR_MSG := ERROR: Invalid .pak file and/or extract directory and/or file list
define CHECK_EXTRACT_ERROR =
$(if $(pak),,$(error $(EXTRACT_ERROR_MSG)))
$(if $(dir),,$(error $(EXTRACT_ERROR_MSG)))
$(if $(filelist),,$(error $(EXTRACT_ERROR_MSG)))
endef

.PHONY: check_extract_error
check_extract_error:
	$(call CHECK_EXTRACT_ERROR)

.PHONY: extract
extract: all check_extract_error
	mkdir -vp $(dir)
	./$(TARGET).out $(pak) -e $(dir) $(filelist)

.PHONY: extract_all
extract_all: extract_ae1 extract_ae2

.PHONY: extract_ae1
extract_ae1:
	$(MAKE) extract pak=$(AE1pak_pak) dir=$(AE1pak_dir) filelist=$(AE1pak_filelist)

.PHONY: extract_ae2
extract_ae2:
	$(MAKE) extract pak=$(AE2pak_pak) dir=$(AE2pak_dir) filelist=$(AE2pak_filelist)


# Clean up extracted files

.PHONY: clean_all_extracted
clean_all_extracted: clean_ae1 clean_ae2

.PHONY: clean_ae1
clean_ae1:
	-rm -rfv $(AE1pak_dir) $(AE1pak_filelist)

.PHONY: clean_ae2
clean_ae2:
	-rm -rfv $(AE2pak_dir) $(AE2pak_filelist)


# Pack .pak

# check error for extraction
PACK_ERROR_MSG := ERROR: Invalid .pak file and/or file list
define CHECK_PACK_ERROR =
$(if $(pak),,$(error $(PACK_ERROR_MSG)))
$(if $(filelist),,$(error $(PACK_ERROR_MSG)))
endef

.PHONY: pack
pack: all
	$(call CHECK_PACK_ERROR)
	./$(TARGET).out $(pak) -p $(filelist)

pack_check_exec := pack_check.sh

.PHONY: pack_all
pack_all: pack_ae1 pack_ae2

.PHONY: pack_ae1
pack_ae1:
	bash $(pack_check_exec) $(AE1pak_pak) $(AE1pak_filelist)

.PHONY: pack_ae2
pack_ae2:
	bash $(pack_check_exec) $(AE2pak_pak) $(AE2pak_filelist)
