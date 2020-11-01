# This Makefile extracts the map editors for AE1 and AE2.

TARGETS = $(AE1MAP) $(AE2MAP) $(AE2LANG)
AE1MAP := AE1map
AE2MAP := AE2map
AE2LANG := AE2lang
AE2PAK := AE2pak

.PHONY: all
all: $(TARGETS)

.PHONY: $(AE1MAP)
$(AE1MAP): AE1map20090913.rar
	unrar x -yo $^

.PHONY: $(AE2MAP)
$(AE2MAP): AE2map081111.zip
	unzip -o $^

.PHONY: $(AE2LANG)
$(AE2LANG):
	$(MAKE) -C $@/

.PHONY: clean
clean:
	-rm -rfv $(AE1MAP) $(AE2MAP)
	$(MAKE) -C $(AE2LANG)/ $@
