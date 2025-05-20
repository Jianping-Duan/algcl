CC ?= cc
AR ?= ar
RMF = -rm -f

CFLAGS = -std=c18 -O2 -Wall -W -Wfloat-equal -Wfloat-conversion \
		-Wdouble-promotion -m64
CFLAGS += $(DEBUG)
CFLAGS += $(CFLAGS_AUX)

INCLUDE_PATH = -I $(TOPDIR)/include
LIBRARY_PATH = -L $(TOPDIR)/lib
LIBRARY_PATH2 = $(TOPDIR)/lib
BINARY_PATH = $(TOPDIR)/bin

.PHONY: all
all: $(SLIBS) $(EXECS)

$(EXECS): $(.PREFIX).c
	$(CC) $(.ALLSRC) -o $(BINARY_PATH)/$(.TARGET) $(CFLAGS) \
		$(INCLUDE_PATH) $(LIBRARY_PATH) $(CLIB) $(LIBS) 

$(SLIBS): $(OBJS)
	$(AR) crsv $(LIBRARY_PATH2)/$(SLIBS) $(OBJS)
#	$(RMF) $(OBJS)
	
$(OBJS): $(.PREFIX).c
	$(CC) -c $(.ALLSRC) -o $(.TARGET) $(CFLAGS) $(INCLUDE_PATH)
	
.PHONY: clean
clean:
	$(RMF) $(OBJS) $(LIBRARY_PATH2)/$(SLIBS) 
.for n in $(EXECS)
	$(RMF) $(BINARY_PATH)/$(n)
.endfor
