cmake_minimum_required(VERSION 3.27)

project(LibraryLinkUtilities
        VERSION 1.0
        DESCRIPTION "Extended utilities for LibraryLink and WSTP"
        HOMEPAGE_URL "https://github.com/chenty0704/LibraryLinkUtilities"
        LANGUAGES CXX)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

find_package(LLU REQUIRED CONFIG)
find_package(System REQUIRED CONFIG)

include_directories(include)

add_subdirectory(src)

install(FILES cmake/LibraryLinkUtilitiesConfig.cmake
        DESTINATION share/LibraryLinkUtilities)
