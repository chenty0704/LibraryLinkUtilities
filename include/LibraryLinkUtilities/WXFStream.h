#pragma once

namespace LLU {
    /// \brief Provides a stream-like interface to read WXF files.
    class InWXFStream {
        enum class WXFType : unsigned char {
            Function = 'f',
            Int8 = 'C',
            Int16 = 'j',
            Int32 = 'i',
            Int64 = 'L',
            Real = 'r',
            String = 'S',
            Symbol = 's',
            PackedArray = 193
        };

        enum class WXFArrayType : unsigned char {
            Int = 3,
            Real = 35
        };

    public:
        /// \brief Creates a stream associated with a WXF file.
        /// \param path The path to the WXF file.
        explicit InWXFStream(const filesystem::path &path) : _stream(path, ios::binary) {
            if (!_stream.is_open()) throw runtime_error("Invalid file name.");

            string header(2, '\0');
            ReadArray(header.data(), static_cast<int>(header.length()));
            if (header != "8:") throw runtime_error("Invalid header.");

            CheckType(WXFType::Function);
            _length = ReadLength();
            CheckSymbol("List");
        }

        /// \brief Returns the number of parts in the WXF file.
        /// \return The number of parts in the WXF file.
        [[nodiscard]] int Length() const noexcept {
            return _length;
        }

        template<typename T> requires is_integral_v<T>
        InWXFStream &operator>>(T &val) {
            if (const auto type = Read<WXFType>(); type == WXFType::Int8) val = static_cast<T>(Read<int8_t>());
            else if (type == WXFType::Int16) val = static_cast<T>(Read<int16_t>());
            else if (type == WXFType::Int32) val = static_cast<T>(Read<int32_t>());
            else if (type == WXFType::Int64) val = static_cast<T>(Read<int64_t>());
            else throw runtime_error("Invalid type.");
            return *this;
        }

        InWXFStream &operator>>(double &val) {
            CheckType(WXFType::Real);
            val = Read<double>();
            return *this;
        }

        InWXFStream &operator>>(vector<double> &vec) {
            CheckType(WXFType::PackedArray);
            CheckArrayType(WXFArrayType::Real);
            if (ReadLength() != 1) throw runtime_error("Invalid rank.");
            const auto len = ReadLength();
            vec.resize(len);
            ReadArray(vec.data(), len);
            return *this;
        }

        template<typename T>
        InWXFStream &operator>>(vector<T> &vec) {
            CheckType(WXFType::PackedArray);
            Read<WXFArrayType>();
            const auto dims = ReadDimensions();
            vec.resize(dims.front());
            ReadArray(vec.data(), static_cast<int>(vec.size()));
            return *this;
        }

        template<typename Extents>
        InWXFStream &operator>>(mdarray<double, Extents> &arr) {
            CheckType(WXFType::PackedArray);
            CheckArrayType(WXFArrayType::Real);
            static constexpr int rank = Extents().rank();
            const auto dims = ReadDimensions<rank>();
            arr = apply([](auto... sizes) { return mdarray<double, Extents>(sizes...); }, dims);
            ReadArray(arr.data(), ranges::fold_left(dims, 1, multiplies()));
            return *this;
        }

        template<typename T, typename Extents>
        InWXFStream &operator>>(mdarray<T, Extents> &arr) {
            CheckType(WXFType::PackedArray);
            Read<WXFArrayType>();
            static constexpr int rank = Extents().rank();
            const auto dims = ReadDimensions();
            array<int, rank> _dims;
            copy_n(dims.cbegin(), rank, _dims.begin());
            arr = apply([](auto... sizes) { return mdarray<T, Extents>(sizes...); }, _dims);
            ReadArray(arr.data(), ranges::fold_left(_dims, 1, multiplies()));
            return *this;
        }

    private:
        template<typename T>
        T Read() {
            T obj;
            _stream.read(reinterpret_cast<char *>(&obj), sizeof(T));
            return obj;
        }

        template<typename T>
        void ReadArray(T *arr, int len) {
            _stream.read(reinterpret_cast<char *>(arr), len * sizeof(T));
        }

        int ReadLength() {
            auto len = 0;
            for (auto i = 0; i < sizeof(int); ++i) {
                const auto b = Read<byte>();
                if (b >> 7 == byte{0}) {
                    len += to_integer<int>(b) << i * 7;
                    break;
                }
                len += to_integer<int>(b & ~(byte{1} << 7)) << i * 7;
            }
            return len;
        }

        template<int rank>
        array<int, rank> ReadDimensions() {
            if (ReadLength() != rank) throw runtime_error("Invalid rank.");
            array<int, rank> dims;
            for (auto i = 0; i < rank; ++i) dims[i] = ReadLength();
            return dims;
        }

        vector<int> ReadDimensions() {
            const auto rank = ReadLength();
            vector<int> dims(rank);
            for (auto i = 0; i < rank; ++i) dims[i] = ReadLength();
            return dims;
        }

        void CheckType(WXFType type) {
            if (Read<WXFType>() != type) throw runtime_error("Invalid type.");
        }

        void CheckArrayType(WXFArrayType type) {
            if (Read<WXFArrayType>() != type) throw runtime_error("Invalid type.");
        }

        void CheckSymbol(string_view name) {
            CheckType(WXFType::Symbol);
            const auto len = ReadLength();
            if (len != name.length()) throw runtime_error("Invalid length.");
            string str(len, '\0');
            ReadArray(str.data(), len);
            if (str != name) throw runtime_error("Invalid symbol.");
        }

        ifstream _stream;
        int _length;
    };
}
