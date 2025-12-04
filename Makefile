# Makefile -- script to build cont program.
# Author: Luis Colorado <luiscoloradourcola@gmail.com>
# Date: Sun Sep  8 13:10:15 EEST 2024
# Copyright: (c) 2024 Luis Colorado.  All rights reserved.

RM     ?= rm -f

prefix ?= /usr/local

bindir ?= $(prefix)/bin
datarootdir ?= $(prefix)/share
mandir ?= $(datarootdir)/man
man1   ?= $(mandir)/man1

INSTALL ?= install

OWN   ?= root
GRP   ?= bin
FMOD  ?= 0444
XMOD  ?= 0111
DMOD  ?= 0755

IFLAGS ?= -o $(OWN) -g $(GRP)

CFLAGS += -DVERSION=\""v0.11"\"

targets = cont

cont_objs = cont.o
cont_libs = -ltermcap

all: $(targets)
clean:
	rm -f $(toclean)

install: $(targets)
	-$(INSTALL) $(IFLAGS) -m $(DMOD) -d $(bindir)
	-$(INSTALL) $(IFLAGS) -m $(DMOD) -d $(man1)
	-$(INSTALL) $(IFLAGS) -m $(XMOD) cont $(bindir)/cont
	-$(INSTALL) $(IFLAGS) -m $(FMOD) cont.1 $(man1)/cont.1

uninstall:
	-$(RM) $(bindir)/cont
	-$(RM) $(man1)/cont.1

toclean += cont $(cont_objs)
cont: $(cont_deps) $(cont_objs)
	$(CC) $(CFLAGS) -o $@ $(cont_objs) $(cont_ldflags) $(cont_libs) $(LIBS)
