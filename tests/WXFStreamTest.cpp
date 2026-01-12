#include <gtest/gtest.h>

import System.Base;
import System.MDArray;

import LibraryLinkUtilities.WXFStream;

using namespace std;
using namespace experimental;

TEST(InWXFStreamTest, BasicInput) {
    LLU::InWXFStream stream("resources/TestData.wxf");
    EXPECT_EQ(stream.Length(), 4);

    int64_t intVal;
    stream >> intVal;
    EXPECT_EQ(intVal, 1024);

    double realVal;
    stream >> realVal;
    EXPECT_EQ(realVal, 3.14);

    vector<int64_t> list;
    stream >> list;
    EXPECT_EQ(list, views::iota(0, 10) | ranges::to<vector<int64_t>>());

    mdarray<int64_t, dims<2>> matrix;
    stream >> matrix;
    EXPECT_EQ(matrix.container(), vector<int64_t>({
                  1, 0, 0,
                  0, 1, 0,
                  0, 0, 1}));
}
