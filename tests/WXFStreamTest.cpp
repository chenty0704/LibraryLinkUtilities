#include <gtest/gtest.h>

import System.Base;
import System.MDArray;

import LibraryLinkUtilities.WXFStream;

using namespace std;
using namespace experimental;

TEST(InWXFStreamTest, BasicInput) {
    LLU::InWXFStream stream("resources/TestData.wxf");
    EXPECT_EQ(stream.Length(), 4);

    int intValue;
    stream >> intValue;
    EXPECT_EQ(intValue, 1024);

    double realValue;
    stream >> realValue;
    EXPECT_EQ(realValue, 3.14);

    vector<int> values;
    stream >> values;
    EXPECT_EQ(values, views::iota(0, 10) | ranges::to<vector>());

    mdarray<int, dims<2>> matrix;
    stream >> matrix;
    EXPECT_EQ(matrix.container(), vector({
                  1, 0, 0,
                  0, 1, 0,
                  0, 0, 1}));
}
