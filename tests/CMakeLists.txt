add_executable(LibraryLinkUtilitiesTest
        "TimeSeriesTest.cpp"
        "WXFStreamTest.cpp")
target_link_libraries(LibraryLinkUtilitiesTest PRIVATE
        GTest::gtest_main
        LibraryLinkUtilities)
