MAKE ?= make

.PHONY: all
all: $(SUBDIRS:%=%-recursion-all)

.PHONY: $(SUBDIRS:%=%-recursion-all)
$(SUBDIRS:%=%-recursion-all):
	$(MAKE) -C $(subst -recursion-all,,$@) all

.PHONY: clean
clean: $(SUBDIRS:%=%-recursion-clean)

.PHONY: $(SUBDIRS:%=%-recursion-clean)
$(SUBDIRS:%=%-recursion-clean):
	$(MAKE) -C $(subst -recursion-clean,,$@) clean
