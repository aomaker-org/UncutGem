cmake_minimum_required(VERSION 3.10)
project(FirmwareTests)

set(CMAKE_CXX_STANDARD 11)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/mocks)
include_directories(${CMAKE_SOURCE_DIR}/../src)

# Mock sources
set(MOCK_SOURCES
    ${CMAKE_SOURCE_DIR}/mocks/Arduino.cpp
)

# Test sources
set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test_adf4350.cpp
    ${CMAKE_BINARY_DIR}/adf4350_test.cpp
)

# Custom command to generate the stripped source file before compiling
file(WRITE ${CMAKE_BINARY_DIR}/strip_include.sh
"#!/bin/sh\n"
"sed 's/#include <sweep_array.h>//g' \"$1\" | sed 's/#include \"sweep_array.h\"//g' > \"$2\"\n"
)
execute_process(COMMAND chmod +x ${CMAKE_BINARY_DIR}/strip_include.sh)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/adf4350_test.cpp
    COMMAND ${CMAKE_BINARY_DIR}/strip_include.sh ${CMAKE_SOURCE_DIR}/../src/adf4350.cpp ${CMAKE_BINARY_DIR}/adf4350_test.cpp
    DEPENDS ${CMAKE_SOURCE_DIR}/../src/adf4350.cpp
)

# Add GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(run_tests ${TEST_SOURCES} ${MOCK_SOURCES})
target_link_libraries(run_tests ${GTEST_LIBRARIES} pthread)
