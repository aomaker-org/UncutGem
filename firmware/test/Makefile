CXX = g++
CXXFLAGS = -std=c++11 -Wall -I. -I../src

TARGET = run_tests

SRCS = test_calibration.cpp ../src/calibration.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

test: $(TARGET)
	./$(TARGET)
