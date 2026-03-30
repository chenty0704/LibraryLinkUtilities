export module LibraryLinkUtilities.MArgumentQueue;

import System.Base;

import LibraryLinkUtilities.Base;

using namespace std;

namespace LLU {
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
            if constexpr (!requires { typename MArgumentManager::CustomType<T>::CorrespondingTypes; }) ++_index;
            else _index += tuple_size_v<typename MArgumentManager::CustomType<T>::CorrespondingTypes>;
            return value;
        }

        /// Removes the first tensor in the queue.
        /// @tparam T The type of values.
        /// @tparam Mode The mode of passing.
        /// @returns The first tensor in the queue.
        template<typename T, Passing Mode = Passing::Automatic>
        Tensor<T> PopTensor() {
            return _argManager.getTensor<T, Mode>(_index++);
        }

        /// Removes the first data vector in the queue.
        /// @tparam Mode The mode of passing.
        /// @returns The first data vector in the queue.
        template<Passing Mode = Passing::Automatic>
        DataVector PopDataVector() {
            return _argManager.getDataVector<Mode>(_index++);
        }

        /// Sets the output of the library function.
        /// @param value The output of the library function.
        void SetOutput(const auto &value) {
            _argManager.set(value);
        }
    };
}
