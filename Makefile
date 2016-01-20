CFLAGS=-std=c99
all:
	cc $(CFLAGS) -o ACtrlGroupIden ACtrlGroupIden.c -I /usr/local/include/pbc -L /usr/local/lib -Wl,-rpath /usr/local/lib  -l pbc -l gmp
	cc $(CFLAGS) -o DHKeyExchange DHKeyExchange.c -I /usr/local/include/pbc -L /usr/local/lib -Wl,-rpath /usr/local/lib  -l pbc -l gmp
clean:
	rm ACtrlGroupIden
	rm DHKeyExchange
