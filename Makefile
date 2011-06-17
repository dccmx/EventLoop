#
# nezhad - Makefile
#
# Author: dccmx <dccmx@dccmx.com>
#

VERSION    = 0.1

OBJFILES   = eventloop.cc
INCFILES   = eventloop.h

CFLAGS_GEN = -Wall -g $(CFLAGS) -DVERSION=\"$(VERSION)\"
CFLAGS_DBG = -ggdb $(CFLAGS_GEN)
CFLAGS_OPT = -O3 -Wno-format -Wno-unused-result $(CFLAGS_GEN)

LDFLAGS   += 
LIBS      += 

all: echoserver example
	@echo
	@echo "Make Complete. See README for how to use."
	@echo
	@echo "Having problems with it? Send complains and bugs to dccmx@dccmx.com"
	@echo

example: example.cc $(OBJFILES) $(INCFILES)
	$(CXX) $(LDFLAGS) -o example $(CFLAGS_OPT) $(LIBS) $^

echoserver: example.cc $(OBJFILES) $(INCFILES)
	$(CXX) $(LDFLAGS) -o echoserver $(CFLAGS_OPT) $(LIBS) $^

clean:
	rm -f example echoserver core core.[1-9][0-9]* memcheck.out callgrind.out.[1-9][0-9]* massif.out.[1-9][0-9]*
