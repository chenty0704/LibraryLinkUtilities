#pragma once

#include <LLU/LLU.h>

#include "LibraryLinkUtilities/DataTypes.h"
#include "LibraryLinkUtilities/MArgumentQueue.h"
#include "LibraryLinkUtilities/WXFStream.h"

namespace LLU {
    /// Represents the types of LibraryLink function errors.
    inline vector<ErrorManager::ErrorStringData> PacletErrors = {
        {"InvalidArgumentError", "Invalid argument `arg`."},
    };

    template<>
    struct MArgumentManager::Getter<string_view> {
        static string_view get(const MArgumentManager &manager, size_t i) {
            return manager.get<const char *>(i);
        }
    };

    inline WSStream<WS::Encoding::UTF8> &operator<<(WSStream<WS::Encoding::UTF8> &stream, string_view str) {
        WS::String<WS::Encoding::UTF8>::put(stream.get(), str.data(), static_cast<int>(str.length()));
        return stream;
    }

    template<DescribedStruct T>
    struct MArgumentManager::Getter<T> {
        static T get(const MArgumentManager &manager, size_t i) {
            const auto obj = manager.get<string_view>(i);
            try {
                return JSON::Deserialize<T>(obj);
            } catch (...) { ErrorManager::throwException("InvalidArgumentError", obj); }
        }
    };

    // @formatter:off

    #define LLU_TRY_DESERIALIZE(r, data, Derived) \
        if (type == BOOST_PP_STRINGIZE(Derived)) \
            return make_unique<Derived>(JSON::Deserialize<Derived>(obj));

    /// \brief Defines a LibraryLink getter for an abstract struct.
    /// \param C An abstract struct.
    /// \param Derived A list of derived structs.
    #define LLU_DEFINE_ABSTRACT_STRUCT_GETTER(C, Derived) \
        namespace LLU { \
            template<> \
            struct TypeInformation<unique_ptr<C>> { \
                static constexpr int RawArgumentCount = 2; \
            }; \
            \
            template<> \
            struct MArgumentManager::Getter<unique_ptr<C>> { \
                static unique_ptr<C> get(const MArgumentManager &manager, size_t i) { \
                    const auto [type, obj] = manager.getTuple<string_view, string_view>(i); \
                    try { \
                        BOOST_PP_SEQ_FOR_EACH(LLU_TRY_DESERIALIZE, _, BOOST_PP_TUPLE_TO_SEQ(Derived)) \
                    } catch (...) { ErrorManager::throwException("InvalidArgumentError", obj); } \
                    ErrorManager::throwException("InvalidArgumentError", type); \
                } \
            }; \
        }

    // @formatter:on

    template<typename T>
    struct TypeInformation<TimeSeriesView<T>> {
        static constexpr int RawArgumentCount = 2;
    };

    template<typename T>
    struct MArgumentManager::Getter<TimeSeriesView<T>> {
        static TimeSeriesView<T> get(const MArgumentManager &manager, size_t i) {
            const auto intervalSeconds = manager.get<double>(i);
            const auto vals = manager.getGenericTensor<Passing::Constant>(i + 1);
            const auto *const dims = vals.getDimensions();
            const auto pathLen = static_cast<int>(dims[0]);
            const mdspan _vals(static_cast<const T *>(vals.rawData()), pathLen);
            return {intervalSeconds, _vals};
        }
    };

    template<typename T>
    struct TypeInformation<TemporalDataView<T>> {
        static constexpr int RawArgumentCount = 2;
    };

    template<typename T>
    struct MArgumentManager::Getter<TemporalDataView<T>> {
        static TemporalDataView<T> get(const MArgumentManager &manager, size_t i) {
            const auto intervalSeconds = manager.get<double>(i);
            const auto vals = manager.getGenericTensor<Passing::Constant>(i + 1);
            const auto *const dims = vals.getDimensions();
            const auto pathCount = static_cast<int>(dims[0]), pathLen = static_cast<int>(dims[1]);
            const mdspan _vals(static_cast<const T *>(vals.rawData()), pathCount, pathLen);
            return {intervalSeconds, _vals};
        }
    };
}
