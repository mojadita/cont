# Makefile -- script to build cont program.
# Author: Luis Colorado <luiscoloradourcola@gmail.com>
# Date: Sun Sep  8 13:10:15 EEST 2024
# Copyright: (c) 2024 Luis Colorado.  All rights reserved.

RM     ?= rm -f

prefix ?= /usr/local

bindir ?= $(prefix)/bin
mandir ?= $(prefix)/man
man1   ?= $(mandir)/man1

INSTALL ?= install

CFLAGS += -DVERSION=\""v0.11"\"

targets = cont

cont_objs = cont.o
cont_libs = -ltermcap

all: $(targets)
clean:
	rm -f $(toclean)

install: $(targets)
	-$(INSTALL) $(IFLAGS) -d $(bindir)
	-$(INSTALL) $(IFLAGS) -d $(man1)
	-$(INSTALL) $(IFLAGS) cont $(bindir)/cont
	-$(INSTALL) $(IFLAGS) cont.1 $(man1)/cont.1

uninstall:
	-$(RM) $(bindir)/cont
	-$(RM) $(man1)/cont.1

toclean += cont $(cont_objs)
cont: $(cont_deps) $(cont_objs)
	$(CC) $(CFLAGS) -o $@ $(cont_objs) $(cont_ldflags) $(cont_libs) $(LIBS)
