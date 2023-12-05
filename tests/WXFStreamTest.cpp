#include <gtest/gtest.h>

#include "LibraryLinkUtilities/WXFStream.h"

TEST(InWXFStreamTest, BasicInput) {
    LLU::InWXFStream stream("resources/TestData.wxf");
    EXPECT_EQ(stream.Length(), 4);

    int intVal;
    stream >> intVal;
    EXPECT_EQ(intVal, 1024);

    double realVal;
    stream >> realVal;
    EXPECT_EQ(realVal, 3.14);

    vector<double> vec;
    stream >> vec;
    EXPECT_EQ(vec, views::iota(1, 11) | ranges::to<vector<double>>());

    mdarray<double, dextents<int, 2>> arr;
    stream >> arr;
    EXPECT_EQ(arr.container(), vector<double>({1, 0, 0, 0, 1, 0, 0, 0, 1}));
}
