module;

#include <LLU/LLU.h>

export module LibraryLinkUtilities.Base;

import System.Base;
import System.JSON;

using namespace std;

export {
    using ::MArgument;
    using ::WolframLibraryData;
}

export namespace LLU {
    using LLU::ErrorManager;
    using LLU::GenericTensor;
    using LLU::LibraryData;
    using LLU::LibraryLinkError;
    using LLU::MArgumentManager;
    using LLU::Passing;
    using LLU::Tensor;
    using LLU::WSStream;

    namespace ErrorCode {
        using ErrorCode::DimensionsError;
        using ErrorCode::FunctionError;
        using ErrorCode::MemoryError;
        using ErrorCode::NoError;
        using ErrorCode::NumericalError;
        using ErrorCode::RankError;
        using ErrorCode::TypeError;
        using ErrorCode::VersionError;
    }

    namespace WS {
        using WS::Encoding;
        using WS::String;

        template struct PutScalar<wsint64>;
    }
}

namespace LLU {
    /// Represents the types of LibraryLink function errors.
    export vector<ErrorManager::ErrorStringData> PacletErrors = {
        {"InvalidArgumentError", "Invalid argument `arg`."},
    };

    export template<>
    struct MArgumentManager::Getter<string_view> {
        [[nodiscard]] static string_view get(const MArgumentManager &argManager, size_t index) {
            return argManager.get<const char *>(index);
        }
    };

    export WSStream<WS::Encoding::UTF8> &operator<<(WSStream<WS::Encoding::UTF8> &stream, string_view value) {
        WS::String<WS::Encoding::UTF8>::put(stream.get(), value.data(), static_cast<int>(value.length()));
        return stream;
    }

    export template<DescribedStruct T>
    struct MArgumentManager::Getter<T> {
        [[nodiscard]] static T get(const MArgumentManager &argManager, size_t index) {
            const auto value = argManager.get<string_view>(index);
            try {
                return JSON::Deserialize<T>(value);
            } catch (...) { ErrorManager::throwException("InvalidArgumentError", value); }
        }
    };
}
