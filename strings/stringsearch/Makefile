# DEBUG = -Og -g

TOPDIR = ../..
LIBS = -ldigraph -llinearlist -lalgcomm

OBJS = trie.o tstrie.o stringsearch.o nfaregexp.o stringsuffixes.o \
	   longestsubstring.o
SLIBS = libalgstrings.a
CLIB = -lalgstrings
EXECS = trietest tst strsearch nfare grep strsuff kwic lsubs

.include "$(TOPDIR)/algcode.mk"
