set(HEADER_DIR ${PROJECT_SOURCE_DIR}/include)

add_library(LibraryLinkUtilities SHARED
        LibraryLinkUtilities.cpp)
target_include_directories(LibraryLinkUtilities PUBLIC $<INSTALL_INTERFACE:include>)
target_sources(LibraryLinkUtilities PUBLIC FILE_SET HEADERS BASE_DIRS ${HEADER_DIR} FILES
        ${HEADER_DIR}/LibraryLinkUtilities/DataTypes.h
        ${HEADER_DIR}/LibraryLinkUtilities/LibraryLinkUtilities.h)
target_link_libraries(LibraryLinkUtilities PUBLIC
        LLU::LLU
        System::System)

install(TARGETS LibraryLinkUtilities EXPORT LibraryLinkUtilitiesTargets
        ARCHIVE
        FILE_SET HEADERS)
install(EXPORT LibraryLinkUtilitiesTargets DESTINATION share/LibraryLinkUtilities
        NAMESPACE LibraryLinkUtilities::)
