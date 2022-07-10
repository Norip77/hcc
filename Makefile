CFLAGS=c11 -g -static

hcc: main.c

test: hcc
	./test.sh

clean:
	rm -f hcc *.o *~ tmp*

.PHONY: test clean