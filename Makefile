CFLAGS=-std=c11 -g -fno-common
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

hcc: $(OBJS)
	$(CC) -o hcc $(OBJS) $(LDFLAGS)

$(OBJS): hcc.h

test: hcc
	./test.sh

clean:
	rm -f hcc *.o *~ tmp* *.out

.PHONY: test clean