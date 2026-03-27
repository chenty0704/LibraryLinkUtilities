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

    /// Represents a constant view of a regular time series.
    /// @tparam T The type of values.
    export template<typename T>
    struct TimeSeriesView {
        double IntervalSeconds; ///< The interval between two values in seconds.
        span<const T> Values; ///< A list of values.

        /// Returns the length of the path.
        /// @returns The length of the path.
        [[nodiscard]] int PathLength() const {
            return static_cast<int>(Values.size());
        }

        /// Returns the duration of the path in seconds.
        /// @returns The duration of the path in seconds.
        [[nodiscard]] double DurationSeconds() const {
            return PathLength() * IntervalSeconds;
        }
    };

    /// Represents a constant view of a collection of regular time series.
    /// @tparam T The type of values.
    export template<typename T>
    struct TemporalDataView {
        double IntervalSeconds; ///< The interval between two values in seconds.
        mdspan<const T, dims<2>> Values; ///< A 2D array of values.

        /// Returns the number of paths in the collection.
        /// @returns The number of paths in the collection.
        [[nodiscard]] int PathCount() const {
            return static_cast<int>(Values.extent(0));
        }

        /// Returns the length of each path.
        /// @returns The length of each path.
        [[nodiscard]] int PathLength() const {
            return static_cast<int>(Values.extent(1));
        }

        /// Returns the duration of each path in seconds.
        /// @returns The duration of each path in seconds.
        [[nodiscard]] double DurationSeconds() const {
            return PathLength() * IntervalSeconds;
        }

        /// Returns the path at the specified index.
        /// @param index The index of the path.
        /// @returns The path at the specified index.
        [[nodiscard]] TimeSeriesView<T> operator[](int index) const {
            return {IntervalSeconds, span(&Values[index, 0], PathLength())};
        }
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

    export template<DescribedStruct T>
    struct MArgumentManager::CustomType<unique_ptr<T>> {
        using CorrespondingTypes = tuple<string>;
    };

    export template<DescribedStruct T>
    struct MArgumentManager::Getter<unique_ptr<T>> {
        [[nodiscard]] static unique_ptr<T> get(const MArgumentManager &argManager, size_t index) {
            const auto value = argManager.get<string>(index);
            try {
                return JSON::Deserialize<unique_ptr<T>>(value);
            } catch (...) { ErrorManager::throwException("InvalidArgumentError", value); }
        }
    };

    export template<typename T>
    struct MArgumentManager::CustomType<TimeSeriesView<T>> {
        using CorrespondingTypes = tuple<double, Managed<GenericTensor, Passing::Constant>>;
    };

    export template<typename T>
    struct MArgumentManager::Getter<TimeSeriesView<T>> {
        [[nodiscard]] static TimeSeriesView<T> get(const MArgumentManager &argManager, size_t index) {
            const auto intervalSeconds = argManager.get<double>(index);
            const auto values = argManager.get<Managed<GenericTensor, Passing::Constant>>(index + 1);
            const auto *const dimensions = values.getDimensions();
            const auto pathLength = static_cast<int>(dimensions[0]);
            const span _values(static_cast<const T *>(values.rawData()), pathLength);
            return {intervalSeconds, _values};
        }
    };

    export template<typename T>
    struct MArgumentManager::CustomType<TemporalDataView<T>> {
        using CorrespondingTypes = tuple<double, Managed<GenericTensor, Passing::Constant>>;
    };

    export template<typename T>
    struct MArgumentManager::Getter<TemporalDataView<T>> {
        [[nodiscard]] static TemporalDataView<T> get(const MArgumentManager &argManager, size_t index) {
            const auto intervalSeconds = argManager.get<double>(index);
            const auto values = argManager.get<Managed<GenericTensor, Passing::Constant>>(index + 1);
            const auto *const dimensions = values.getDimensions();
            const auto pathCount = static_cast<int>(dimensions[0]), pathLength = static_cast<int>(dimensions[1]);
            const mdspan<const T, dims<2>> _values(static_cast<const T *>(values.rawData()), pathCount, pathLength);
            return {intervalSeconds, _values};
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
