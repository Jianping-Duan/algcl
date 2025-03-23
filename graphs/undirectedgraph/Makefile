# DEBUG = -O0 -g 

TOPDIR = ../..
LIBS = -lsearchtree -lheap -llinearlist -lalgcomm 

OBJS = graph.o graphgenerator.o graphdfsp.o graphbfsp.o \
	graphconcomp.o graphcycle.o euleriancycle.o \
	eulerianpath.o bipartitegraphdfs.o \
	bipartitegraphbfs.o symbolgraph.o bipartitematching.o \
	hopcroftkarp.o
SLIBS = libundigraph.a
CLIB = -lundigraph
EXECS = graphtest gdfsp gbfsp graphcc gcycle eulcycletest \
	eulpathtest bigraphdfstest bigraphbfstest \
	symbolgraphtest bipmatch hkbipmatch

.include "$(TOPDIR)/algcode.mk"
