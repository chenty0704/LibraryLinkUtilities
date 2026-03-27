add_executable(LibraryLinkUtilitiesTest
        "WXFStreamTest.cpp")
target_link_libraries(LibraryLinkUtilitiesTest PRIVATE
        GTest::gtest_main
        LibraryLinkUtilities)
