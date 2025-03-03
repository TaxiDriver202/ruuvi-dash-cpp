#ruuvi-dash-cpp

# edit to fit your machine
CROWINC = -I/usr/include/crow
#
CXXFLAGS= -std=c++17 -O2 $(CROWINC)
CXX=g++

ruuvidash: ruuvidash.o
	$(CXX) -o ruuvidash ruuvidash.o
ruuvidash.o: ruuvidash.cpp
	$(CXX) -c -lpthread $(CXXFLAGS) -o ruuvidash.o ruuvidash.cpp

clean:
	rm ruuvidash.o

