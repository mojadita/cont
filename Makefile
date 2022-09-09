prefix ?= $(HOME)

bindir ?= $(prefix)/bin
mandir ?= $(prefix)/man
man1   ?= $(mandir)/man1

INSTALL ?= install

CFLAGS += -DVERSION=\""v0.10"\"

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

toclean += cont $(cont_objs)
cont: $(cont_deps) $(cont_objs)
	$(CC) $(CFLAGS) -o $@ $(cont_objs) $(cont_ldflags) $(cont_libs) $(LIBS)
