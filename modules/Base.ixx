module;

#include <LLU/LLU.h>

export module LibraryLinkUtilities.Base;

import System.Base;
import System.JSON;
import System.MDArray;
import System.Reflection;

using namespace std;

export {
    using ::MArgument;
    using ::WolframLibraryData;
}

export namespace LLU {
    using LLU::DataList;
    using LLU::ErrorManager;
    using LLU::GenericTensor;
    using LLU::LibraryData;
    using LLU::LibraryLinkError;
    using LLU::MArgumentManager;
    using LLU::Passing;
    using LLU::Tensor;

    namespace Argument::Typed {
        using Typed::Any;
    }

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

    namespace NodeType = Argument::Typed;

    namespace WS {
        template struct PutScalar<wsint64>;
    }
}

namespace LLU {
    /// Represents the types of LibraryLink function errors.
    export vector<ErrorManager::ErrorStringData> PacletErrors = {
        {"NativeError", "``"},
        {"InvalidArgumentError", "Invalid argument `arg`."},
    };

    export template<>
    struct MArgumentManager::CustomType<filesystem::path> {
        using CorrespondingTypes = tuple<string>;
    };

    export template<DescribedStruct T>
    struct MArgumentManager::CustomType<T> {
        using CorrespondingTypes = tuple<string>;
    };

    export template<DescribedStruct T>
    struct MArgumentManager::Getter<T> {
        [[nodiscard]] static T get(const MArgumentManager &argManager, size_t index) {
            const auto value = argManager.get<string>(index);
            try {
                return JSON::Deserialize<T>(value);
            } catch (...) { ErrorManager::throwException("InvalidArgumentError", value); }
        }
    };

    /// Converts a LibraryLink tensor to a multidimensional span.
    /// @tparam T The type of elements.
    /// @tparam Extents The type of dimensions.
    /// @param values A LibraryLink tensor.
    /// @returns A multidimensional span of the same elements.
    export template<typename T, typename Extents>
    [[nodiscard]] mdspan<T, Extents> ToMDSpan(Tensor<T> &values) {
        if (values.rank() != static_cast<int>(Extents::rank())) throw runtime_error("Invalid rank.");

        using IndexType = Extents::index_type;
        array<IndexType, Extents::rank()> dimensions;
        ranges::transform(values.dimensions().get(), dimensions.begin(),
                          [](int64_t dimension) { return static_cast<IndexType>(dimension); });
        return {values.data(), dimensions};
    }

    /// Converts a constant tensor to a constant multidimensional span.
    /// @tparam T The type of values.
    /// @tparam Extents The type of dimensions.
    /// @param values A constant tensor.
    /// @returns A constant multidimensional span of the same elements.
    export template<typename T, typename Extents>
    [[nodiscard]] mdspan<const T, Extents> ToMDSpan(const Tensor<T> &values) {
        if (values.rank() != static_cast<int>(Extents::rank())) throw runtime_error("Invalid rank.");

        using IndexType = Extents::index_type;
        array<IndexType, Extents::rank()> dimensions;
        ranges::transform(values.dimensions().get(), dimensions.begin(),
                          [](int64_t dimension) { return static_cast<IndexType>(dimension); });
        return {values.data(), dimensions};
    }

    /// Tries to invoke a function with LibraryLink-specific error handling.
    /// @tparam Fun The type of the function.
    /// @param function A function to invoke.
    /// @returns A LibraryLink error code.
    export template<invocable Fun>
    int TryInvoke(Fun function) {
        try {
            function();
        } catch (const LibraryLinkError &e) {
            return e.id();
        } catch (const exception &e) {
            try {
                ErrorManager::throwException("NativeError", e.what());
            } catch (const LibraryLinkError &_e) { return _e.id(); }
        }
        return ErrorCode::NoError;
    }
}
