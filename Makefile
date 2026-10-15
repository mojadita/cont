# Makefile -- script to build cont program.
# Author: Luis Colorado <luiscoloradourcola@gmail.com>
# Date: Sun Sep  8 13:10:15 EEST 2024
# Copyright: (c) 2024 Luis Colorado.  All rights reserved.

RM            ?= rm -f

prefix        ?= /usr/local

OS            != uname -s

bindir        ?= $(prefix)/bin
datarootdir   ?= $(prefix)/share
mandir        ?= $(datarootdir)/man
man1dir       ?= $(mandir)/man1

INSTALL       ?= install

OWN-FreeBSD   ?= -o root
GRP-FreeBSD   ?= -g wheel

OWN-GNU/Linux ?= -o bin
GRP-GNU/Linux ?= -g bin

OWN           ?= $(OWN-$(OS))
GRP           ?= $(GRP-$(OS))
FMOD          ?= -m 0444
XMOD          ?= -m 0111
DMOD          ?= -m 0755

IFLAGS        ?= $(OWN) $(GRP)

CFLAGS        += -DVERSION=\""v0.12"\"

targets        = cont

cont_deps      =
cont_objs      = cont.o
cont_libs      = -ltermcap
cont_ldfl      =
to_clean      += $(cont_objs)

all: $(targets)
clean:
	rm -f $(to_clean)

to_install     = $(bindir)/cont   \
                 $(man1dir)/cont.1.gz
to_clean      += cont cont.1.gz

install: $(to_install)

$(bindir)/cont: $(@:T)
	-$(INSTALL) $(IFLAGS) $(XMOD) $(@:T) $@

$(man1dir)/cont.1.gz: $(@:T)
	-$(INSTALL) $(IFLAGS) $(FMOD) $(@:T) $@

cont.1.gz: cont.1
	gzip -v < cont.1 > $@

uninstall remove deinstall:
	-$(RM) $(to_install)

cont: $(cont_deps) $(cont_objs)
	$(CC) $(CFLAGS) -o $@ $(cont_objs) \
		$(cont_ldflags) $(cont_libs) $(LIBS)
