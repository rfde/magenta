all: magenta

magenta: magenta.cc magenta.hh main.cc
	$(CXX) -o magenta -Wall magenta.cc main.cc

clean:
	rm -f magenta