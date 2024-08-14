include(CMakeFindDependencyMacro)

find_dependency(LLU REQUIRED CONFIG)
find_dependency(System REQUIRED CONFIG)

include("${CMAKE_CURRENT_LIST_DIR}/LibraryLinkUtilitiesTargets.cmake")
