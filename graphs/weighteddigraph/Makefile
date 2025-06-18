# DEBUG = -O0 -g
TOPDIR = ../..
LIBS = -lheap -llinearlist -lalgcomm -lm 

OBJS = edgeweighteddigraph.o dijkstrasp.o \
	dijkstrapairsp.o ewdigraphdfso.o \
	ewdigraphcycle.o ewditopological.o acyclicsp.o \
	bellmanfordsp.o adjmatrixewdigraph.o \
	floydwarshallsp.o
SLIBS = libewdigraph.a
CLIB = -lewdigraph
EXECS = ewdigraph djkrsp djkrpsp ewdgdfso ewdgc \
	ewdtplg acycsp bmfsp adjmatewdg fwsp

.include "$(TOPDIR)/algcode.mk"
