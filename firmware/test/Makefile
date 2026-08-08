# Makefile for doctest tests
CXX = g++
CXXFLAGS = -std=c++11 -I../src -I.

SRC = test_adf4350.cpp test_stubs.cpp
OBJ = $(SRC:.cpp=.o)

all: test_adf4350

test_adf4350: $(OBJ)
	$(CXX) $(CXXFLAGS) -o test_adf4350 $(OBJ)

clean:
	rm -f test_adf4350 $(OBJ)

test: all
	./test_adf4350
