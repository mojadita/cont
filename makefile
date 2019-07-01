targets = cont

cont_objs = cont.o

all: $(targets)
clean:
	rm -f $(toclean)

.for t in $(targets)
toclean += $t $($t_objs)
$t: $($t_deps) $($t_objs)
	$(CC) $(CFLAGS) -o $@ $($t_objs) $($t_ldflags) $($t_libs) $(LIBS)
.endfor
