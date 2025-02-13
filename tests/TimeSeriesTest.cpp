#include <gtest/gtest.h>

import System.Base;

import LibraryLinkUtilities.TimeSeries;

using namespace std;

TEST(TimeSeriesTest, Window) {
    const vector values = {0, 1, 2, 3};
    const LLU::TimeSeriesView<int> timeSeries = {1., values};

    EXPECT_TRUE(ranges::equal(timeSeries.Window(0.5, 1.75).Values, vector({1, 2})));
    EXPECT_TRUE(ranges::equal(timeSeries.Window(0.5, 1.5).Values, vector({1})));
}
