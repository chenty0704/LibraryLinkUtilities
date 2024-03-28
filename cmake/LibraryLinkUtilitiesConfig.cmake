include(CMakeFindDependencyMacro)

set(Boost_NO_WARN_NEW_VERSIONS ON)

find_dependency(Boost REQUIRED CONFIG)
find_dependency(LLU REQUIRED CONFIG)
find_dependency(System REQUIRED CONFIG)

include("${CMAKE_CURRENT_LIST_DIR}/LibraryLinkUtilitiesTargets.cmake")
