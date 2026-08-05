CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I. -I../src
SRCS = $(wildcard *.cpp)
OBJS = test_adf4350.o adf4350_wrapper.o sweep_array.o
TEST_BIN = test_runner

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

test_adf4350.o: test_adf4350.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

adf4350_wrapper.o: adf4350_wrapper.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

sweep_array.o: sweep_array.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TEST_BIN)
