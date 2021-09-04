# This Makefile performs several tasks:
# - Extract map editors for AE1 and AE2.
# - Compile AE2lang tool, which is used to convert and edit the language files for AE1 and AE2.
# - Compile AE2pak tool, which is used to convert and edit the resource files for AE1 and AE2.

TARGETS := AE1map AE2map AE2lang AE2pak AE2units

AE1MAP_zip := AE1map20090913.zip
AE2MAP_zip := AE2map081111.zip
AE2MAP_test := AE2testmap.zip

.PHONY: all
all: $(TARGETS)

.PHONY: AE1map
AE1map: $(AE1MAP_zip)
	unzip -o $<

.PHONY: AE2map
AE2map: $(AE2MAP_zip) $(AE2MAP_test)
	unzip -o $<
	unzip -o $(word 2,$^) -d "$@"

.PHONY: AE2lang
AE2lang:
	$(MAKE) -C $@/

.PHONY: AE2pak
AE2pak:
	$(MAKE) -C $@/

.PHONY: AE2units
AE2units:
	$(MAKE) -C $@/

.PHONY: clean
clean:
	-rm -rf AE1map
	-rm -rf AE2map
	$(MAKE) -C AE2lang $@
	$(MAKE) -C AE2pak $@
	$(MAKE) -C AE2units
	$(MAKE) -C utils $@