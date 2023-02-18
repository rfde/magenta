all: magenta libmagenta.so magenta_shared

magenta: magenta.cc magenta.hh main.cc
	$(CXX) -o magenta -Wall magenta.cc main.cc

libmagenta.so: magenta.cc magenta.hh
	$(CXX) -c magenta.cc
	$(CC) -shared -o libmagenta.so magenta.o

magenta_shared: magenta_shared.c
	$(CC) -o magenta_shared -L. -lmagenta magenta_shared.c

clean:
	rm -f magenta
	rm -f magenta.o
	rm -f libmagenta.so
	rm -f magenta_shared
