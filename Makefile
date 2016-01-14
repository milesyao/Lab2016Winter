all:
	cc -o ACtrlGroupIden ACtrlGroupIden.c -I /usr/local/include/pbc -L /usr/local/lib -Wl,-rpath /usr/local/lib  -l pbc -l gmp

clean:
	rm ACtrlGroupIden
