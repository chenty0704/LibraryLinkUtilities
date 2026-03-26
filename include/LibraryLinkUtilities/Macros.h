#pragma once

#include <System/Macros.h>

/// Registers a time series type for LibraryLink passing.
/// @param T The type of values.
#define LLU_REGISTER_TIME_SERIES_TYPE(T) \
    template<> \
    struct LLU::MArgumentManager::CustomType<LLU::TimeSeriesView<T>> { \
        using CorrespondingTypes = tuple<double, Managed<GenericTensor, Passing::Constant>>; \
    }; \
    \
    template<> \
    struct LLU::MArgumentManager::Getter<LLU::TimeSeriesView<T>> { \
        [[nodiscard]] static TimeSeriesView<T> get(const MArgumentManager &argManager, size_t index) { \
            const auto intervalSeconds = argManager.get<double>(index); \
            const auto values = argManager.get<Managed<GenericTensor, Passing::Constant>>(index + 1); \
            const auto *const dimensions = values.getDimensions(); \
            const auto pathLength = static_cast<int>(dimensions[0]); \
            const span<const T> _values(static_cast<const T *>(values.rawData()), pathLength); \
            return {intervalSeconds, _values}; \
        } \
    }; \
    \
    template<> \
    struct LLU::MArgumentManager::CustomType<LLU::TemporalDataView<T>> { \
        using CorrespondingTypes = tuple<double, Managed<GenericTensor, Passing::Constant>>; \
    }; \
    \
    template<> \
    struct LLU::MArgumentManager::Getter<LLU::TemporalDataView<T>> { \
        [[nodiscard]] static TemporalDataView<T> get(const MArgumentManager &argManager, size_t index) { \
            const auto intervalSeconds = argManager.get<double>(index); \
            const auto values = argManager.get<Managed<GenericTensor, Passing::Constant>>(index + 1); \
            const auto *const dimensions = values.getDimensions(); \
            const auto pathCount = static_cast<int>(dimensions[0]), pathLength = static_cast<int>(dimensions[1]); \
            const mdspan<const T, dims<2>> _values(static_cast<const T *>(values.rawData()), pathCount, pathLength); \
            return {intervalSeconds, _values}; \
        } \
    };
