add_library(LibraryLinkUtilities)
target_sources(LibraryLinkUtilities PUBLIC FILE_SET CXX_MODULES FILES
        "Base.ixx"
        "MArgumentQueue.ixx"
        "WXFStream.ixx")
target_link_libraries(LibraryLinkUtilities PUBLIC
        LLU::LLU
        System::System)

install(TARGETS LibraryLinkUtilities EXPORT LibraryLinkUtilitiesTargets
        FILE_SET CXX_MODULES DESTINATION "modules/LibraryLinkUtilities")
install(EXPORT LibraryLinkUtilitiesTargets DESTINATION "share/LibraryLinkUtilities"
        NAMESPACE LibraryLinkUtilities::)
