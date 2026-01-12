export module LibraryLinkUtilities.WXFStream;

import System.Base;
import System.MDArray;

using namespace std;
using namespace experimental;

namespace LLU {
    /// Provides a stream-like interface to read WXF files.
    export class InWXFStream {
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
            Int8 = 0,
            Int16 = 1,
            Int32 = 2,
            Int64 = 3,
            Real = 35
        };

        ifstream _stream;
        int _length;

    public:
        /// Creates a stream associated with a WXF file.
        /// @param path The path to the WXF file.
        explicit InWXFStream(const filesystem::path &path) : _stream(path, ios::binary) {
            if (!_stream.is_open()) throw runtime_error("Invalid path.");

            string header(2, '\0');
            ReadArray(header.data(), static_cast<int>(header.length()));
            if (header != "8:") throw runtime_error("Invalid header.");

            CheckType(WXFType::Function);
            _length = ReadLength();
            CheckSymbol("List");
        }

        /// Returns the number of parts in the WXF file.
        /// @returns The number of parts in the WXF file.
        [[nodiscard]] int Length() const {
            return _length;
        }

        InWXFStream &operator>>(int64_t &value) {
            const auto type = Read<WXFType>();
            if (type == WXFType::Int8) value = Read<int8_t>();
            else if (type == WXFType::Int16) value = Read<int16_t>();
            else if (type == WXFType::Int32) value = Read<int32_t>();
            else if (type == WXFType::Int64) value = Read<int64_t>();
            else throw runtime_error("Invalid type.");
            return *this;
        }

        InWXFStream &operator>>(double &value) {
            CheckType(WXFType::Real);
            value = Read<double>();
            return *this;
        }

        InWXFStream &operator>>(vector<int64_t> &values) {
            CheckType(WXFType::PackedArray);
            const auto arrayType = Read<WXFArrayType>();
            if (ReadLength() != 1) throw runtime_error("Invalid rank.");
            const auto length = ReadLength();

            values.resize(length);
            if (arrayType == WXFArrayType::Int8) {
                vector<int8_t> buffer(length);
                ReadArray(buffer.data(), length);
                ranges::copy(buffer, values.begin());
            } else if (arrayType == WXFArrayType::Int16) {
                vector<int16_t> buffer(length);
                ReadArray(buffer.data(), length);
                ranges::copy(buffer, values.begin());
            } else if (arrayType == WXFArrayType::Int32) {
                vector<int32_t> buffer(length);
                ReadArray(buffer.data(), length);
                ranges::copy(buffer, values.begin());
            } else if (arrayType == WXFArrayType::Int64) ReadArray(values.data(), length);
            else throw runtime_error("Invalid array type.");
            return *this;
        }

        InWXFStream &operator>>(vector<double> &values) {
            CheckType(WXFType::PackedArray);
            CheckArrayType(WXFArrayType::Real);
            if (ReadLength() != 1) throw runtime_error("Invalid rank.");
            const auto length = ReadLength();

            values.resize(length);
            ReadArray(values.data(), length);
            return *this;
        }

        template<typename T>
        InWXFStream &operator>>(vector<T> &values) {
            CheckType(WXFType::Function);
            const auto length = ReadLength();
            CheckSymbol("List");

            values.resize(length);
            for (auto &value : values) *this >> value;
            return *this;
        }

        template<typename Extents>
        InWXFStream &operator>>(mdarray<int64_t, Extents> &values) {
            CheckType(WXFType::PackedArray);
            const auto arrayType = Read<WXFArrayType>();
            static constexpr auto rank = static_cast<int>(Extents::rank());
            const auto dimensions = ReadDimensions<rank>();
            const auto size = ranges::fold_left(dimensions, 1, multiplies());

            values = apply([](auto... args) { return mdarray<int64_t, Extents>(args...); }, dimensions);
            if (arrayType == WXFArrayType::Int8) {
                vector<int8_t> buffer(size);
                ReadArray(buffer.data(), size);
                ranges::copy(buffer, values.data());
            } else if (arrayType == WXFArrayType::Int16) {
                vector<int16_t> buffer(size);
                ReadArray(buffer.data(), size);
                ranges::copy(buffer, values.data());
            } else if (arrayType == WXFArrayType::Int32) {
                vector<int32_t> buffer(size);
                ReadArray(buffer.data(), size);
                ranges::copy(buffer, values.data());
            } else if (arrayType == WXFArrayType::Int64) ReadArray(values.data(), size);
            else throw runtime_error("Invalid array type.");
            return *this;
        }

        template<typename Extents>
        InWXFStream &operator>>(mdarray<double, Extents> &values) {
            CheckType(WXFType::PackedArray);
            CheckArrayType(WXFArrayType::Real);
            static constexpr auto rank = static_cast<int>(Extents::rank());
            const auto dimensions = ReadDimensions<rank>();
            const auto size = ranges::fold_left(dimensions, 1, multiplies());

            values = apply([](auto... args) { return mdarray<double, Extents>(args...); }, dimensions);
            ReadArray(values.data(), size);
            return *this;
        }

    private:
        template<typename T>
        T Read() {
            T value;
            _stream.read(reinterpret_cast<char *>(&value), sizeof(T));
            return value;
        }

        int ReadLength() {
            auto length = 0;
            for (auto i = 0; i < sizeof(int); ++i) {
                const auto value = Read<byte>();
                if (value >> 7 == byte{0}) return length + (to_integer<int>(value) << i * 7);
                length += to_integer<int>(value & ~(byte{1} << 7)) << i * 7;
            }
            return length;
        }

        template<int rank>
        array<int, rank> ReadDimensions() {
            if (ReadLength() != rank) throw runtime_error("Invalid rank.");

            array<int, rank> dimensions;
            for (auto &dimension : dimensions) dimension = ReadLength();
            return dimensions;
        }

        template<typename T>
        void ReadArray(T *values, int length) {
            _stream.read(reinterpret_cast<char *>(values), length * sizeof(T));
        }

        void CheckType(WXFType type) {
            if (Read<WXFType>() != type) throw runtime_error("Invalid type.");
        }

        void CheckArrayType(WXFArrayType type) {
            if (Read<WXFArrayType>() != type) throw runtime_error("Invalid array type.");
        }

        void CheckSymbol(string_view name) {
            CheckType(WXFType::Symbol);
            const auto length = ReadLength();
            if (length != name.length()) throw runtime_error("Invalid symbol.");

            string value(length, '\0');
            ReadArray(value.data(), length);
            if (value != name) throw runtime_error("Invalid symbol.");
        }
    };
}
