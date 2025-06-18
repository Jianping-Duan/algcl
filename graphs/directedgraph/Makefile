# DEBUG = -O0 -g

TOPDIR = ../..
LIBS = -lsearchtree -lheap -lskiplist -llinearlist -lalgcomm -lm

OBJS = digraph.o digraphgenerator.o digraphdfs.o \
	digraphdfsp.o digraphbfsp.o digraphcycle.o \
	directedcycle.o directedeulerian.o digraphdfso.o \
	symboldigraph.o ditopological.o kosarajusharirscc.o \
	tarjanscc.o gabowscc.o
SLIBS = libdigraph.a
CLIB = -ldigraph
EXECS = digraphtest dgdfs dgdfsp dgbfsp dgcycle dgcyclex \
	dieulertest dgdfso symboldigraphtest ditplg \
	kjsscc tjscc gbscc

.include "$(TOPDIR)/algcode.mk"
