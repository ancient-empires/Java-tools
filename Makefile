# This Makefile extracts the map editors for AE1 and AE2.

TARGETS = $(AE1MAP) $(AE2MAP)
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

.PHONY: clean
clean:
	-rm -rfv $(TARGETS)
