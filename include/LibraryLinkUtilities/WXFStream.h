#pragma once

namespace LLU {
    /// \brief Provides a stream-like interface to read WXF files.
    class InWXFStream {
        enum class WXFType : unsigned char {
            Function = 'f',
            Int8 = 'C',
            Int16 = 'j',
            Int32 = 'i',
            Real = 'r',
            String = 'S',
            Symbol = 's',
            PackedArray = 193
        };

        enum class WXFArrayType : unsigned char {
            Int8 = 0,
            Int16 = 1,
            Int32 = 2,
            Real = 35
        };

        ifstream _stream;
        int _length;

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

        InWXFStream &operator>>(int &val) {
            if (const auto type = Read<WXFType>(); type == WXFType::Int8) val = Read<int8_t>();
            else if (type == WXFType::Int16) val = Read<int16_t>();
            else if (type == WXFType::Int32) val = Read<int32_t>();
            else throw runtime_error("Invalid type.");
            return *this;
        }

        InWXFStream &operator>>(double &val) {
            CheckType(WXFType::Real);
            val = Read<double>();
            return *this;
        }

        InWXFStream &operator>>(vector<int> &vec) {
            CheckType(WXFType::PackedArray);
            const auto arrType = Read<WXFArrayType>();
            if (ReadLength() != 1) throw runtime_error("Invalid rank.");

            const auto len = ReadLength();
            vec.resize(len);
            if (arrType == WXFArrayType::Int8) {
                vector<int8_t> buffer(len);
                ReadArray(buffer.data(), len);
                ranges::copy(as_const(buffer), vec.begin());
            } else if (arrType == WXFArrayType::Int16) {
                vector<int16_t> buffer(len);
                ReadArray(buffer.data(), len);
                ranges::copy(as_const(buffer), vec.begin());
            } else if (arrType == WXFArrayType::Int32) ReadArray(vec.data(), len);
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
            if (const auto type = Read<WXFType>(); type == WXFType::PackedArray) {
                Read<WXFArrayType>();
                const auto dims = ReadDimensions();
                vec.resize(dims.front());
                ReadArray(vec.data(), static_cast<int>(vec.size()));
            } else if (type == WXFType::Function) {
                const auto len = ReadLength();
                CheckSymbol("List");
                vec.resize(len);
                for (auto &val : vec) *this >> val;
            } else throw runtime_error("Invalid type.");
            return *this;
        }

        template<typename Extents>
        InWXFStream &operator>>(mdarray<int, Extents> &arr) {
            CheckType(WXFType::PackedArray);
            const auto arrType = Read<WXFArrayType>();

            static constexpr int rank = Extents::rank();
            const auto dims = ReadDimensions<rank>();
            const auto size = ranges::fold_left(dims, 1, multiplies());

            arr = apply([](auto... vals) { return mdarray<int, Extents>(vals...); }, dims);
            if (arrType == WXFArrayType::Int8) {
                vector<int8_t> buffer(size);
                ReadArray(buffer.data(), size);
                ranges::copy(as_const(buffer), arr.data());
            } else if (arrType == WXFArrayType::Int16) {
                vector<int16_t> buffer(size);
                ReadArray(buffer.data(), size);
                ranges::copy(as_const(buffer), arr.data());
            } else if (arrType == WXFArrayType::Int32) ReadArray(arr.data(), size);
            return *this;
        }

        template<typename Extents>
        InWXFStream &operator>>(mdarray<double, Extents> &arr) {
            CheckType(WXFType::PackedArray);
            CheckArrayType(WXFArrayType::Real);

            static constexpr int rank = Extents::rank();
            const auto dims = ReadDimensions<rank>();
            const auto size = ranges::fold_left(dims, 1, multiplies());

            arr = apply([](auto... vals) { return mdarray<double, Extents>(vals...); }, dims);
            ReadArray(arr.data(), size);
            return *this;
        }

        template<typename T, typename Extents>
        InWXFStream &operator>>(mdarray<T, Extents> &arr) {
            CheckType(WXFType::PackedArray);
            Read<WXFArrayType>();

            static constexpr int rank = Extents::rank();
            const auto dims = ReadDimensions();
            array<int, rank> _dims;
            copy_n(dims.cbegin(), rank, _dims.begin());
            const auto size = ranges::fold_left(_dims, 1, multiplies());

            arr = apply([](auto... vals) { return mdarray<T, Extents>(vals...); }, _dims);
            ReadArray(arr.data(), size);
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
            if (len != name.length()) throw runtime_error("Invalid symbol.");
            string str(len, '\0');
            ReadArray(str.data(), len);
            if (str != name) throw runtime_error("Invalid symbol.");
        }
    };
}
