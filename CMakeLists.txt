cmake_minimum_required(VERSION 3.31)

project(LibraryLinkUtilities
        VERSION 1.0
        DESCRIPTION "Extended utilities for LibraryLink and WSTP"
        HOMEPAGE_URL "https://github.com/chenty0704/LibraryLinkUtilities"
        LANGUAGES CXX)

find_package(LLU REQUIRED CONFIG)
find_package(System REQUIRED CONFIG)

add_subdirectory("modules")
add_subdirectory("tests")

install(FILES
        "cmake/LibraryLinkUtilitiesConfig.cmake"
        "LibraryLinkUtilities.wl"
        DESTINATION "share/LibraryLinkUtilities")
