export module LibraryLinkUtilities.MArgumentQueue;

import System.Base;

import LibraryLinkUtilities.Base;

namespace LLU {
    /// Provides information about a type that can be requested from LibraryLink.
    /// @tparam T A type that can be requested from LibraryLink.
    export template<typename T>
    struct TypeInfo {
        static constexpr int RawArgumentCount = 1; ///< The number of raw LibraryLink arguments.
    };

    /// Provides a queue-like interface to get LibraryLink arguments.
    export class MArgumentQueue {
        MArgumentManager _argManager;
        int _index = 0;

    public:
        /// Creates a queue of LibraryLink arguments.
        /// @param argc The number of arguments.
        /// @param args The arguments of the library function.
        /// @param out The output of the library function.
        MArgumentQueue(int64_t argc, MArgument *args, MArgument &out) : _argManager(argc, args, out) {
        }

        /// Returns the first argument in the queue.
        /// @tparam T The requested type.
        /// @returns The first argument in the queue.
        template<typename T>
        [[nodiscard]] MArgumentManager::RequestedType<T> Peek() const {
            return _argManager.get<T>(_index);
        }

        /// Removes the first argument in the queue.
        /// @tparam T The requested type.
        /// @returns The first argument in the queue.
        template<typename T>
        MArgumentManager::RequestedType<T> Pop() {
            auto value = _argManager.get<T>(_index);
            _index += TypeInfo<T>::RawArgumentCount;
            return value;
        }

        /// Sets the output of the library function.
        /// @param value The output of the library function.
        void SetOutput(const auto &value) {
            _argManager.set(value);
        }
    };
}
