hcc: main.c
	cc -o hcc main.c

test: hcc
	./test.sh

clean:
	rm -f hcc *.o *~ tmp*

.PHONY: test clean