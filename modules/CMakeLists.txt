set(HEADERS_DIR "${PROJECT_SOURCE_DIR}/include")

add_library(LibraryLinkUtilities)
target_include_directories(LibraryLinkUtilities PUBLIC
        "$<BUILD_INTERFACE:${HEADERS_DIR}>"
        "$<INSTALL_INTERFACE:include>")
target_sources(LibraryLinkUtilities PUBLIC
        FILE_SET HEADERS BASE_DIRS "${HEADERS_DIR}" FILES
        "${HEADERS_DIR}/LibraryLinkUtilities/Macros.h"
        "${HEADERS_DIR}/LLU/ErrorLog/Errors.h"
        FILE_SET CXX_MODULES FILES
        "Base.ixx"
        "MArgumentQueue.ixx"
        "TimeSeries.ixx"
        "WXFStream.ixx")
target_link_libraries(LibraryLinkUtilities PUBLIC
        Boost::headers
        LLU::LLU
        System::System)

install(TARGETS LibraryLinkUtilities EXPORT LibraryLinkUtilitiesTargets
        FILE_SET HEADERS
        FILE_SET CXX_MODULES DESTINATION "modules/LibraryLinkUtilities")
install(EXPORT LibraryLinkUtilitiesTargets DESTINATION "share/LibraryLinkUtilities"
        NAMESPACE LibraryLinkUtilities::)
