MAKE ?= make

.PHONY: all
all: $(SUBDIRS:%=%-recursion-all)

.PHONY: $(SUBDIRS:%=%-recursion-all)
$(SUBDIRS:%=%-recursion-all):
	$(MAKE) -C $(.TARGET:%-recursion-all=%) all

.PHONY: clean
clean: $(SUBDIRS:%=%-recursion-clean)

.PHONY: $(SUBDIRS:%=%-recursion-clean)
$(SUBDIRS:%=%-recursion-clean):
	$(MAKE) -C $(.TARGET:%-recursion-clean=%) clean
