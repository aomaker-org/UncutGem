CXX = g++
CXXFLAGS = -std=c++11 -Wall -I. -I../src

ADC_SRCS = test_adc_utils.cpp ../src/adc_utils.cpp
ADC_OBJS = $(ADC_SRCS:.cpp=.o)
ADC_EXEC = run_adc_tests

CAL_SRCS = test_calibration.cpp ../src/calibration.cpp
CAL_OBJS = $(CAL_SRCS:.cpp=.o)
CAL_EXEC = run_calibration_tests

.PHONY: all clean test

all: test

test: $(ADC_EXEC) $(CAL_EXEC)
	./$(ADC_EXEC)
	./$(CAL_EXEC)

$(ADC_EXEC): $(ADC_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(CAL_EXEC): $(CAL_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(ADC_OBJS) $(ADC_EXEC) $(CAL_OBJS) $(CAL_EXEC)
