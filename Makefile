JAVA := Java

.PHONY: all
all: $(JAVA)

.PHONY: $(JAVA)
$(JAVA):
	$(MAKE) -C $@

.PHONY: clean
clean:
	$(MAKE) -C $(JAVA) $@
