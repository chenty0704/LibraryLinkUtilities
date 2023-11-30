#pragma once

#include <LLU/MArgumentManager.h>

namespace LLU {
    /// \brief Provides information about a type that can be requested from LibraryLink.
    /// \tparam T A type that can be requested from LibraryLink.
    template<typename T>
    struct TypeInformation {
        static constexpr int RawArgumentCount = 1; ///< The number of raw LibraryLink arguments needed.
    };

    /// \brief Provides a queue-like interface to access LibaryLink arguments.
    class MArgumentQueue {
    public:
        /// \brief Creates a queue of LibaryLink arguments.
        /// \param argc The number of arguments.
        /// \param args The arguments of the library function.
        /// \param out The output of the library function.
        MArgumentQueue(mint argc, MArgument *args, MArgument &out) : _argManager(argc, args, out) {
        }

        /// \brief Returns the first argument in the queue.
        /// \tparam T The requested type.
        /// \return The first argument in the queue.
        template<typename T>
        MArgumentManager::RequestedType<T> Peek() const {
            return _argManager.get<T>(_index);
        }

        /// \brief Removes the first argument in the queue.
        /// \tparam T The requested type.
        /// \return The first argument in the queue.
        template<typename T>
        MArgumentManager::RequestedType<T> Pop() {
            auto value = _argManager.get<T>(_index);
            _index += TypeInformation<T>::RawArgumentCount;
            return value;
        }

        /// \brief Sets the output of the library function.
        /// \param value The output of the library function.
        void Set(const auto &value) {
            _argManager.set(value);
        }

    private:
        MArgumentManager _argManager;
        int _index = 0;
    };
}
