CXXFLAGS=-O3
CFLAGS=-O3

all: magenta libmagenta.so magenta_shared

magenta: magenta.cc magenta.hh main.cc
	$(CXX) $(CXXFLAGS) -o magenta -Wall magenta.cc main.cc

libmagenta.so: magenta.cc magenta.hh
	$(CXX) $(CXXFLAGS) -c magenta.cc
	$(CC) $(CFLAGS) -shared -o libmagenta.so magenta.o

magenta_shared: magenta_shared.c
	$(CC) $(CFLAGS) -o magenta_shared -L. -lmagenta magenta_shared.c

clean:
	rm -f magenta
	rm -f magenta.o
	rm -f libmagenta.so
	rm -f magenta_shared
