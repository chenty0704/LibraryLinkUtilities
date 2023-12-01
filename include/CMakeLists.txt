add_library(LibraryLinkUtilities INTERFACE)
target_include_directories(LibraryLinkUtilities INTERFACE $<INSTALL_INTERFACE:include>)
target_sources(LibraryLinkUtilities INTERFACE FILE_SET HEADERS FILES
        LibraryLinkUtilities/DataTypes.h
        LibraryLinkUtilities/LibraryLinkUtilities.h
        LibraryLinkUtilities/MArgumentQueue.h)
target_link_libraries(LibraryLinkUtilities INTERFACE
        LLU::LLU
        System::System)

install(TARGETS LibraryLinkUtilities EXPORT LibraryLinkUtilitiesTargets
        FILE_SET HEADERS)
install(EXPORT LibraryLinkUtilitiesTargets DESTINATION share/LibraryLinkUtilities
        NAMESPACE LibraryLinkUtilities::)
