#requires c++17 or later
CXXFLAGS= -std=c++17
CXX=g++

ruuvidash: ruuvidash.o
	$(CXX) -o ruuvidash ruuvidash.o
ruuvidash.o: ruuvidash.cpp
	$(CXX) -c -lpthread $(CXXFLAGS) -o ruuvidash.o ruuvidash.cpp

clean:
	rm ruuvidash.o

