# This Makefile performs several tasks:
# - Extract map editors for AE1 and AE2.
# - Compile AE2lang tool, which is used to convert and edit the language files for AE1 and AE2.
# - Compile AE2pak tool, which is used to convert and edit the resource files for AE1 and AE2.

TARGETS = $(AE1MAP) $(AE2MAP) $(AE2LANG) $(AE2PAK)
AE1MAP := AE1map
AE2MAP := AE2map
AE2LANG := AE2lang
AE2PAK := AE2pak

UTILS := utils

AE1MAP_zip := AE1map20090913.zip
AE2MAP_zip := AE2map081111.zip

.PHONY: all
all: $(TARGETS)

.PHONY: $(AE1MAP)
$(AE1MAP): $(AE1MAP_zip)
	unzip -o $^

.PHONY: $(AE2MAP)
$(AE2MAP): $(AE2MAP_zip)
	unzip -o $^

.PHONY: $(AE2LANG)
$(AE2LANG):
	$(MAKE) -C $@/

.PHONY: $(AE2PAK)
$(AE2PAK):
	$(MAKE) -C $@/

.PHONY: clean
clean:
	-rm -rfv $(AE1MAP) $(AE2MAP)
	$(MAKE) -C $(AE2LANG)/ $@
	$(MAKE) -C $(AE2PAK)/ $@
	$(MAKE) -C $(UTILS)/ $@
