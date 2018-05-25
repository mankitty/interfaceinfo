.NOTPARALLEL:

PKGNAME=prifinfo
SRCS    = $(wildcard *.c)
OBJS    = $(patsubst %.c, %.o, $(SRCS))

DEFS = -DIPV6
CFLAGS += $(DEFS)
CFLAGS = -g -I$(shell pwd)/include

#######################################################################################
%.d: %.c
	@echo "  [DEP]  $@"
	@$(CC) -MM $(CFLAGS) $< | sed -e 's,\($*\)\.o[ :]*,$(basename $@).o $@: ,g' > $@

%.o: %.c
	@echo "  [CC]   $@"
	@$(CC) $(CFLAGS) -c $< -o $@
########################################################################################

.PHONY: all clean realclean prepare

all: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(PKGNAME) $^ $(LDLIBS)

clean:
	rm -rf $(OBJS) $(PKGNAME) *.o
