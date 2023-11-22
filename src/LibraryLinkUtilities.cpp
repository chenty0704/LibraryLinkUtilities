#include "LibraryLinkUtilities/LibraryLinkUtilities.h"

const vector<LLU::ErrorManager::ErrorStringData> PacletErrors = {
    {"InvalidArgumentError", "Invalid argument `arg`."},
};

int WolframLibrary_initialize(WolframLibraryData libData) {
    try {
        LLU::LibraryData::setLibraryData(libData);
        LLU::ErrorManager::registerPacletErrors(PacletErrors);
    } catch (const LLU::LibraryLinkError &e) { return e.id(); }
    return LLU::ErrorCode::NoError;
}
