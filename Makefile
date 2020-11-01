targets := AE1map AE2map

.PHONY: all
all: $(targets)

.PHONY: AE1map
AE1map: AE1map20090913.rar
	unrar x -yo $^
	
.PHONY: AE2map
AE2map: AE2map081111.zip
	unzip -o $^
	
.PHONY: clean
clean:
	-rm -rfv $(targets)
