# DEBUG = -O0 -g

TOPDIR = ../..
LIBS = -lheap -lunionfind -lsortalg -llinearlist \
	-lalgcomm -lm

OBJS = edgeweightedgraph.o lazyprimmst.o primmst.o \
	kruskalmst.o boruvkamst.o
SLIBS = libewgraph.a
CLIB = -lewgraph
EXECS = ewgraph lpmst pmst kkmst bkmst

.include "$(TOPDIR)/algcode.mk"
