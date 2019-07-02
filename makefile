prefix ?= $(HOME)

bindir ?= $(prefix)/bin
mandir ?= $(prefix)/man
man1   ?= $(mandir)/man1

targets = cont

cont_objs = cont.o

all: $(targets)
clean:
	rm -f $(toclean)
install:
	$(INSTALL) $(IFLAGS) cont $(bindir)/cont
	$(INSTALL) $(IFLAGS) cont.1 $(man1)/cont.1

.for t in $(targets)
toclean += $t $($t_objs)
$t: $($t_deps) $($t_objs)
	$(CC) $(CFLAGS) -o $@ $($t_objs) $($t_ldflags) $($t_libs) $(LIBS)
.endfor
