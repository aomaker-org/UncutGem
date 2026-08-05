#include <gtest/gtest.h>

// Trick to make private member public for testing
#define private public
#include "adf4350.h"
#undef private

// Define external variables used by the mock
extern int currentAnalogValue;
extern int readCount;
extern int loopLimit;
extern bool stuckInLoop;

// Stub out sweepArr so the linker is happy
byte sweepArr[128][6][5];

class ADF4350Test : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset mock state before each test
        currentAnalogValue = 0;
        readCount = 0;
        loopLimit = 0;
        stuckInLoop = false;
    }
};

TEST_F(ADF4350Test, GetAvgADC_NormalOperation) {
    ADF4350 pll(1, 2);
    currentAnalogValue = 1000;

    int result = pll.get_avg_ADC();

    // Should take 6 readings
    EXPECT_EQ(readCount, 6);
    // Average of 6 * 1000 should be 1000
    EXPECT_EQ(result, 1000);
}

TEST_F(ADF4350Test, GetAvgADC_HandlesSpikesAboveMaxVal) {
    ADF4350 pll(1, 2);
    currentAnalogValue = 500;

    // Make the analogRead function return a spike (val > 1500) 2 times, then normal
    stuckInLoop = true;
    loopLimit = 2;

    int result = pll.get_avg_ADC();

    // 6 normal readings + 2 spikes = 8 readings total
    EXPECT_EQ(readCount, 8);
    // The spikes should be ignored, so average is still 500
    EXPECT_EQ(result, 500);
}

TEST_F(ADF4350Test, GetAvgADC_InfiniteLoopPrevention) {
    ADF4350 pll(1, 2);
    currentAnalogValue = 2000; // Always return > MAXVAL

    stuckInLoop = true;
    loopLimit = 1000; // Simulate infinite loop

    int result = pll.get_avg_ADC();

    // Should break out after max retries (10 retries per loop * 6 loops = 60 retries + 6 initial reads = 66 reads total)
    EXPECT_EQ(readCount, 66);
    // Because it caps at MAXVAL (1500), average of 6 capped values is 1500
    EXPECT_EQ(result, 1500);
}

TEST_F(ADF4350Test, GetAvgADC_ZeroValue) {
    ADF4350 pll(1, 2);
    currentAnalogValue = 0;

    int result = pll.get_avg_ADC();

    EXPECT_EQ(readCount, 6);
    EXPECT_EQ(result, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
