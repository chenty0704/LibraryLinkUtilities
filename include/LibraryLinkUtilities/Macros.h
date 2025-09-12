#pragma once

#include <System/Macros.h>

#define LLU_TRY_DESERIALIZE(Derived) \
    if (type == STRINGIZE(Derived)) return make_unique<Derived>(JSON::Deserialize<Derived>(value));

/// Generates a LibraryLink getter for an abstract struct.
/// @param T An abstract struct.
/// @param Derived A list of derived structs.
#define LLU_GENERATE_ABSTRACT_STRUCT_GETTER(T, Derived) \
    template<> \
    struct LLU::TypeInfo<unique_ptr<T>> { \
        static constexpr int RawArgumentCount = 2; \
    }; \
    \
    template<> \
    struct LLU::MArgumentManager::Getter<unique_ptr<T>> { \
        [[nodiscard]] static unique_ptr<T> get(const MArgumentManager &argManager, size_t index) { \
            const auto [type, value] = argManager.getTuple<string, string>(index); \
            try { \
                FOR_EACH(LLU_TRY_DESERIALIZE, Derived) \
            } catch (...) { ErrorManager::throwException("InvalidArgumentError", value); } \
            ErrorManager::throwException("InvalidArgumentError", type); \
        } \
    };

/// Generates LibraryLink getters for the time series and temporal data view of the specified type.
/// @param T The type of values.
#define LLU_GENERATE_TIME_SERIES_VIEW_GETTER(T) \
    template<> \
    struct LLU::TypeInfo<LLU::TimeSeriesView<T>> { \
        static constexpr int RawArgumentCount = 2; \
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
    struct LLU::TypeInfo<LLU::TemporalDataView<T>> { \
        static constexpr int RawArgumentCount = 2; \
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
