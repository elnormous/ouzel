// Ouzel by Elviss Strazdins

#ifndef OUZEL_FORMATS_OBF_HPP
#define OUZEL_FORMATS_OBF_HPP

#include <array>
#include <cstdint>
#include <cstring>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include "../utils/Utils.hpp"

namespace ouzel::obf
{
    class DecodeError final: public std::logic_error
    {
    public:
        using std::logic_error::logic_error;
    };

    class TypeError final: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    class RangeError final: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    class Value final
    {
    public:
        using ByteArray = std::vector<std::uint8_t>;
        using Object = std::map<std::uint32_t, Value>;
        using Array = std::vector<Value>;
        using Dictionary = std::map<std::string, Value>;

        enum class Marker: std::uint8_t
        {
            int8 = 1,
            int16 = 2,
            int32 = 3,
            int64 = 4,
            floatingPoint = 5,
            doublePrecision = 6,
            string = 7,
            longString = 8,
            byteArray = 9,
            object = 10,
            array = 11,
            dictionary = 12
        };

        enum class Type
        {
            integer,
            floatingPoint,
            doublePrecision,
            string,
            byteArray,
            object,
            array,
            dictionary
        };

        Value() = default;
        Value(Type initType): type(initType) {}
        Value(std::uint8_t value):
            type(Type::integer), intValue(value)
        {
        }
        Value(std::uint16_t value):
            type(Type::integer), intValue(value)
        {
        }
        Value(std::uint32_t value):
            type(Type::integer), intValue(value)
        {
        }
        Value(std::uint64_t value):
            type(Type::integer), intValue(value)
        {
        }
        Value(float value): type(Type::floatingPoint), doubleValue(value) {}
        Value(double value): type(Type::doublePrecision), doubleValue(value) {}
        Value(const std::string& value):
            type(Type::string), stringValue(value)
        {
        }
        Value(const ByteArray& value): type(Type::byteArray), byteArrayValue(value) {}
        Value(const Object& value): type(Type::object), objectValue(value) {}
        Value(const Array& value): type(Type::array), arrayValue(value) {}
        Value(const Dictionary& value): type(Type::dictionary), dictionaryValue(value) {}

        Value& operator=(Type newType)
        {
            type = newType;

            switch (type)
            {
                case Type::integer:
                    intValue = 0;
                    break;
                case Type::floatingPoint:
                case Type::doublePrecision:
                    doubleValue = 0.0;
                    break;
                case Type::string:
                    stringValue.clear();
                    break;
                case Type::byteArray:
                    byteArrayValue.clear();
                    break;
                case Type::object:
                    objectValue.clear();
                    break;
                case Type::array:
                    arrayValue.clear();
                    break;
                case Type::dictionary:
                    dictionaryValue.clear();
                    break;
                default:
                    break;
            }

            return *this;
        }

        Value& operator=(std::uint8_t value) noexcept
        {
            type = Type::integer;
            intValue = value;

            return *this;
        }

        Value& operator=(std::uint16_t value) noexcept
        {
            type = Type::integer;
            intValue = value;

            return *this;
        }

        Value& operator=(std::uint32_t value) noexcept
        {
            type = Type::integer;
            intValue = value;

            return *this;
        }

        Value& operator=(std::uint64_t value) noexcept
        {
            type = Type::integer;
            intValue = value;

            return *this;
        }

        Value& operator=(float value) noexcept
        {
            type = Type::floatingPoint;
            doubleValue = value;

            return *this;
        }

        Value& operator=(double value) noexcept
        {
            type = Type::doublePrecision;
            doubleValue = value;

            return *this;
        }

        Value& operator=(const std::string& value)
        {
            type = Type::string;
            stringValue = value;

            return *this;
        }

        Value& operator=(const ByteArray& value)
        {
            type = Type::byteArray;
            byteArrayValue = value;

            return *this;
        }

        Value& operator=(const Object& value)
        {
            type = Type::object;
            objectValue = value;

            return *this;
        }

        Value& operator=(const Array& value)
        {
            type = Type::array;
            arrayValue = value;

            return *this;
        }

        Value& operator=(const Dictionary& value)
        {
            type = Type::dictionary;
            dictionaryValue = value;

            return *this;
        }

        auto getType() const noexcept { return type; }
        auto isIntType() const noexcept { return type == Type::integer; }
        auto isFloatType() const noexcept { return type == Type::floatingPoint || type == Type::doublePrecision; }
        auto isStringType() const noexcept { return type == Type::string; }

        std::uint32_t decode(const std::vector<std::uint8_t>& buffer, std::uint32_t offset)
        {
            const std::uint32_t originalOffset = offset;

            if (buffer.size() - offset < 1)
                throw DecodeError{"Not enough data"};

            Marker marker;
            std::memcpy(&marker, buffer.data() + offset, sizeof(marker));
            offset += 1;

            std::uint32_t ret = 0;

            switch (marker)
            {
                case Marker::int8:
                {
                    type = Type::integer;
                    std::uint8_t int8Value;
                    ret = readInt8(buffer, offset, int8Value);
                    intValue = int8Value;
                    break;
                }
                case Marker::int16:
                {
                    type = Type::integer;
                    std::uint16_t int16Value;
                    ret = readInt16(buffer, offset, int16Value);
                    intValue = int16Value;
                    break;
                }
                case Marker::int32:
                {
                    type = Type::integer;
                    std::uint32_t int32Value;
                    ret = readInt32(buffer, offset, int32Value);
                    intValue = int32Value;
                    break;
                }
                case Marker::int64:
                {
                    type = Type::integer;
                    ret = readInt64(buffer, offset, intValue);
                    break;
                }
                case Marker::floatingPoint:
                {
                    type = Type::floatingPoint;
                    float floatValue;
                    ret = readFloat(buffer, offset, floatValue);
                    doubleValue = floatValue;
                    break;
                }
                case Marker::doublePrecision:
                {
                    type = Type::doublePrecision;
                    ret = readDouble(buffer, offset, doubleValue);
                    break;
                }
                case Marker::string:
                {
                    type = Type::string;
                    ret = readString(buffer, offset, stringValue);
                    break;
                }
                case Marker::longString:
                {
                    type = Type::string;
                    ret = readLongString(buffer, offset, stringValue);
                    break;
                }
                case Marker::byteArray:
                {
                    type = Type::byteArray;
                    ret = readByteArray(buffer, offset, byteArrayValue);
                    break;
                }
                case Marker::object:
                {
                    type = Type::object;
                    ret = readObject(buffer, offset, objectValue);
                    break;
                }
                case Marker::array:
                {
                    type = Type::array;
                    ret = readArray(buffer, offset, arrayValue);
                    break;
                }
                case Marker::dictionary:
                {
                    type = Type::dictionary;
                    ret = readDictionary(buffer, offset, dictionaryValue);
                    break;
                }
                default:
                    throw DecodeError{"Unsupported marker"};
            }

            offset += ret;

            return offset - originalOffset;
        }

        std::uint32_t encode(std::vector<std::uint8_t>& buffer) const
        {
            std::uint32_t size = 0;

            std::uint32_t ret = 0;

            switch (type)
            {
                case Type::integer:
                {
                    if (intValue > std::numeric_limits<std::uint32_t>::max())
                    {
                        buffer.push_back(static_cast<std::uint8_t>(Marker::int64));
                        size += 1;
                        ret = writeInt64(buffer, intValue);
                    }
                    else if (intValue > std::numeric_limits<std::uint16_t>::max())
                    {
                        buffer.push_back(static_cast<std::uint8_t>(Marker::int32));
                        size += 1;
                        ret = writeInt32(buffer, static_cast<std::uint32_t>(intValue));
                    }
                    else if (intValue > std::numeric_limits<std::uint8_t>::max())
                    {
                        buffer.push_back(static_cast<std::uint8_t>(Marker::int16));
                        size += 1;
                        ret = writeInt16(buffer, static_cast<std::uint16_t>(intValue));
                    }
                    else
                    {
                        buffer.push_back(static_cast<std::uint8_t>(Marker::int8));
                        size += 1;
                        ret = writeInt8(buffer, static_cast<std::uint8_t>(intValue));
                    }
                    break;
                }
                case Type::floatingPoint:
                {
                    buffer.push_back(static_cast<std::uint8_t>(Marker::floatingPoint));
                    size += 1;
                    ret = writeFloat(buffer, static_cast<float>(doubleValue));
                    break;
                }
                case Type::doublePrecision:
                {
                    buffer.push_back(static_cast<std::uint8_t>(Marker::doublePrecision));
                    size += 1;
                    ret = writeDouble(buffer, doubleValue);
                    break;
                }
                case Type::string:
                {
                    if (stringValue.length() > std::numeric_limits<std::uint16_t>::max())
                    {
                        buffer.push_back(static_cast<std::uint8_t>(Marker::longString));
                        size += 1;
                        ret = writeLongString(buffer, stringValue);
                    }
                    else
                    {
                        buffer.push_back(static_cast<std::uint8_t>(Marker::string));
                        size += 1;
                        ret = writeString(buffer, stringValue);
                    }
                    break;
                }
                case Type::byteArray:
                {
                    buffer.push_back(static_cast<std::uint8_t>(Marker::byteArray));
                    size += 1;
                    ret = writeByteArray(buffer, byteArrayValue);
                    break;
                }
                case Type::object:
                {
                    buffer.push_back(static_cast<std::uint8_t>(Marker::object));
                    size += 1;
                    ret = writeObject(buffer, objectValue);
                    break;
                }
                case Type::array:
                {
                    buffer.push_back(static_cast<std::uint8_t>(Marker::array));
                    size += 1;
                    ret = writeArray(buffer, arrayValue);
                    break;
                }
                case Type::dictionary:
                {
                    buffer.push_back(static_cast<std::uint8_t>(Marker::dictionary));
                    size += 1;
                    ret = writeDictionary(buffer, dictionaryValue);
                    break;
                }
                default:
                    throw std::runtime_error("Unsupported type");
            }

            size += ret;

            return size;
        }

        bool operator!()
        {
            switch (type)
            {
                case Type::integer:
                    return intValue == 0;
                case Type::floatingPoint:
                case Type::doublePrecision:
                    return doubleValue == 0.0;
                case Type::string:
                case Type::byteArray:
                case Type::object:
                case Type::array:
                case Type::dictionary:
                    return false;
                default:
                    return true;
            }
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, std::string>>* = nullptr>
        const std::string& as() const
        {
            if (type != Type::string) throw TypeError{"Wrong type"};
            return stringValue;
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, const char*>>* = nullptr>
        const char* as() const
        {
            if (type != Type::string) throw TypeError{"Wrong type"};
            return stringValue.c_str();
        }

        template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr>
        T as() const
        {
            if (type != Type::integer) throw TypeError{"Wrong type"};
            return static_cast<T>(intValue);
        }

        template <typename T, typename std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
        T as() const
        {
            if (type != Type::floatingPoint && type != Type::doublePrecision)
                throw TypeError{"Wrong type"};
            return static_cast<T>(doubleValue);
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, ByteArray>>* = nullptr>
        T& as()
        {
            type = Type::byteArray;
            return byteArrayValue;
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, ByteArray>>* = nullptr>
        const T& as() const
        {
            if (type != Type::byteArray) throw TypeError{"Wrong type"};
            return byteArrayValue;
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Object>>* = nullptr>
        T& as()
        {
            type = Type::object;
            return objectValue;
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Object>>* = nullptr>
        const T& as() const
        {
            if (type != Type::object) throw TypeError{"Wrong type"};
            return objectValue;
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Array>>* = nullptr>
        T& as()
        {
            type = Type::array;
            return arrayValue;
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Array>>* = nullptr>
        const T& as() const
        {
            if (type != Type::array) throw TypeError{"Wrong type"};
            return arrayValue;
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Dictionary>>* = nullptr>
        T& as()
        {
            type = Type::dictionary;
            return dictionaryValue;
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Dictionary>>* = nullptr>
        const T& as() const
        {
            if (type != Type::dictionary) throw TypeError{"Wrong type"};
            return dictionaryValue;
        }

        Array::iterator begin()
        {
            if (type != Type::array) throw TypeError{"Wrong type"};
            return arrayValue.begin();
        }

        Array::const_iterator begin() const
        {
            if (type != Type::array) throw TypeError{"Wrong type"};
            return arrayValue.begin();
        }

        Array::iterator end()
        {
            if (type != Type::array) throw TypeError{"Wrong type"};
            return arrayValue.end();
        }

        Array::const_iterator end() const
        {
            if (type != Type::array) throw TypeError{"Wrong type"};
            return arrayValue.end();
        }

        auto getSize() const
        {
            if (type != Type::array) throw TypeError{"Wrong type"};
            return static_cast<std::uint32_t>(arrayValue.size());
        }

        const Value& operator[](std::uint32_t key) const
        {
            if (type == Type::object)
            {
                const auto i = objectValue.find(key);

                if (i != objectValue.end())
                    return i->second;
                else
                    throw RangeError{"Index out of range"};
            }
            else if (type == Type::array)
            {
                if (key < arrayValue.size())
                    return arrayValue[key];
                else
                    throw RangeError{"Index out of range"};
            }
            else
                throw TypeError{"Wrong type"};
        }

        Value& operator[](std::uint32_t key)
        {
            if (type != Type::object && type != Type::array) throw TypeError{"Wrong type"};

            if (type == Type::object)
                return objectValue[key];
            else
            {
                type = Type::array;
                return arrayValue[key];
            }
        }

        const Value& operator[](const std::string& key) const
        {
            if (type != Type::dictionary) throw TypeError{"Wrong type"};

            const auto i = dictionaryValue.find(key);

            if (i != dictionaryValue.end())
                return i->second;
            else
                throw RangeError{"Invalid key"};
        }

        Value& operator[](const std::string& key)
        {
            if (type != Type::dictionary) throw TypeError{"Wrong type"};

            return dictionaryValue[key];
        }

        bool hasElement(std::uint32_t key) const
        {
            if (type != Type::object && type != Type::array) throw TypeError{"Wrong type"};

            if (type == Type::object)
                return objectValue.find(key) != objectValue.end();
            else if (type == Type::array)
                return key < arrayValue.size();
            else
                return false;
        }

        bool hasElement(const std::string& key) const
        {
            if (type != Type::dictionary) throw TypeError{"Wrong type"};

            return dictionaryValue.find(key) != dictionaryValue.end();
        }

        void append(const Value& node)
        {
            arrayValue.push_back(node);
        }

    private:
        // reading
        static std::uint32_t readInt8(const std::vector<std::uint8_t>& buffer,
                                      std::uint32_t offset,
                                      std::uint8_t& result)
        {
            if (buffer.size() - offset < sizeof(result))
                throw DecodeError{"Not enough data"};

            std::memcpy(&result, buffer.data() + offset, sizeof(result));

            return sizeof(result);
        }

        static std::uint32_t readInt16(const std::vector<std::uint8_t>& buffer,
                                       std::uint32_t offset,
                                       std::uint16_t& result)
        {
            if (buffer.size() - offset < sizeof(result))
                throw DecodeError{"Not enough data"};

            result = decodeBigEndian<std::uint16_t>(buffer.data() + offset);

            return sizeof(result);
        }

        static std::uint32_t readInt32(const std::vector<std::uint8_t>& buffer,
                                       std::uint32_t offset,
                                       std::uint32_t& result)
        {
            if (buffer.size() - offset < sizeof(result))
                throw DecodeError{"Not enough data"};

            result = decodeBigEndian<std::uint32_t>(buffer.data() + offset);

            return sizeof(result);
        }

        static std::uint32_t readInt64(const std::vector<std::uint8_t>& buffer,
                                       std::uint32_t offset,
                                       std::uint64_t& result)
        {
            if (buffer.size() - offset < sizeof(result))
                throw DecodeError{"Not enough data"};

            result = decodeBigEndian<std::uint64_t>(buffer.data() + offset);

            return sizeof(result);
        }

        static std::uint32_t readFloat(const std::vector<std::uint8_t>& buffer,
                                       std::uint32_t offset,
                                       float& result)
        {
            if (buffer.size() - offset < sizeof(float))
                throw DecodeError{"Not enough data"};

            std::memcpy(&result, buffer.data() + offset, sizeof(result));

            return sizeof(result);
        }

        static std::uint32_t readDouble(const std::vector<std::uint8_t>& buffer,
                                        std::uint32_t offset,
                                        double& result)
        {
            if (buffer.size() - offset < sizeof(double))
                throw DecodeError{"Not enough data"};

            std::memcpy(&result, buffer.data() + offset, sizeof(result));

            return sizeof(result);
        }

        static std::uint32_t readString(const std::vector<std::uint8_t>& buffer,
                                        std::uint32_t offset,
                                        std::string& result)
        {
            const std::uint32_t originalOffset = offset;

            if (buffer.size() - offset < sizeof(std::uint16_t))
                throw DecodeError{"Not enough data"};

            const std::uint16_t length = decodeBigEndian<std::uint16_t>(buffer.data() + offset);

            offset += sizeof(length);

            if (buffer.size() - offset < length)
                throw DecodeError{"Not enough data"};

            result.assign(reinterpret_cast<const char*>(buffer.data() + offset), length);
            offset += length;

            return offset - originalOffset;
        }

        static std::uint32_t readLongString(const std::vector<std::uint8_t>& buffer,
                                            std::uint32_t offset,
                                            std::string& result)
        {
            const std::uint32_t originalOffset = offset;

            if (buffer.size() - offset < sizeof(std::uint32_t))
                throw DecodeError{"Not enough data"};

            const std::uint32_t length = decodeBigEndian<std::uint32_t>(buffer.data() + offset);

            offset += sizeof(length);

            if (buffer.size() - offset < length)
                throw DecodeError{"Not enough data"};

            result.assign(reinterpret_cast<const char*>(buffer.data() + offset), length);
            offset += length;

            return offset - originalOffset;
        }

        static std::uint32_t readByteArray(const std::vector<std::uint8_t>& buffer,
                                           std::uint32_t offset,
                                           ByteArray& result)
        {
            const std::uint32_t originalOffset = offset;

            if (buffer.size() - offset < sizeof(std::uint32_t))
                throw DecodeError{"Not enough data"};

            const std::uint32_t length = decodeBigEndian<std::uint32_t>(buffer.data() + offset);

            offset += sizeof(length);

            if (buffer.size() - offset < length)
                throw DecodeError{"Not enough data"};

            result.assign(reinterpret_cast<const std::uint8_t*>(buffer.data() + offset),
                          reinterpret_cast<const std::uint8_t*>(buffer.data() + offset) + length);
            offset += length;

            return offset - originalOffset;
        }

        static std::uint32_t readObject(const std::vector<std::uint8_t>& buffer,
                                        std::uint32_t offset,
                                        Object& result)
        {
            const std::uint32_t originalOffset = offset;

            if (buffer.size() - offset < sizeof(std::uint32_t))
                throw DecodeError{"Not enough data"};

            const std::uint32_t count = decodeBigEndian<std::uint32_t>(buffer.data() + offset);

            offset += sizeof(count);

            for (std::uint32_t i = 0; i < count; ++i)
            {
                if (buffer.size() - offset < sizeof(std::uint32_t))
                    throw DecodeError{"Not enough data"};

                const std::uint32_t key = decodeBigEndian<std::uint32_t>(buffer.data() + offset);

                offset += sizeof(key);

                Value node;

                const std::uint32_t ret = node.decode(buffer, offset);

                offset += ret;

                result[static_cast<std::uint32_t>(key)] = node;
            }

            return offset - originalOffset;
        }

        static std::uint32_t readArray(const std::vector<std::uint8_t>& buffer,
                                       std::uint32_t offset,
                                       Array& result)
        {
            const std::uint32_t originalOffset = offset;

            if (buffer.size() - offset < sizeof(std::uint32_t))
                throw DecodeError{"Not enough data"};

            const std::uint32_t count = decodeBigEndian<std::uint32_t>(buffer.data() + offset);

            offset += sizeof(count);

            for (std::uint32_t i = 0; i < count; ++i)
            {
                Value node;
                const std::uint32_t ret = node.decode(buffer, offset);

                offset += ret;

                result.push_back(node);
            }

            return offset - originalOffset;
        }

        static std::uint32_t readDictionary(const std::vector<std::uint8_t>& buffer,
                                            std::uint32_t offset,
                                            Dictionary& result)
        {
            const std::uint32_t originalOffset = offset;

            if (buffer.size() - offset < sizeof(std::uint32_t))
                throw DecodeError{"Not enough data"};

            const std::uint32_t count = decodeBigEndian<std::uint32_t>(buffer.data() + offset);

            offset += sizeof(count);

            for (std::uint32_t i = 0; i < count; ++i)
            {
                if (buffer.size() - offset < sizeof(std::uint16_t))
                    throw DecodeError{"Not enough data"};

                const std::uint16_t length = decodeBigEndian<std::uint16_t>(buffer.data() + offset);

                offset += sizeof(length);

                if (buffer.size() - offset < length)
                    throw DecodeError{"Not enough data"};

                std::string key(reinterpret_cast<const char*>(buffer.data() + offset), length);
                offset += length;

                Value node;

                const std::uint32_t ret = node.decode(buffer, offset);

                offset += ret;

                result[key] = node;
            }

            return offset - originalOffset;
        }

        // writing
        static std::uint32_t writeInt8(std::vector<std::uint8_t>& buffer, std::uint8_t value)
        {
            buffer.push_back(value);

            return sizeof(value);
        }

        static std::uint32_t writeInt16(std::vector<std::uint8_t>& buffer, std::uint16_t value)
        {
            std::array<std::uint8_t, sizeof(value)> data;

            encodeBigEndian<std::uint16_t>(data.data(), value);

            buffer.insert(buffer.end(), data.begin(), data.end());

            return sizeof(value);
        }

        static std::uint32_t writeInt32(std::vector<std::uint8_t>& buffer, std::uint32_t value)
        {
            std::array<std::uint8_t, sizeof(value)> data;

            encodeBigEndian<std::uint32_t>(data.data(), value);

            buffer.insert(buffer.end(), data.begin(), data.end());

            return sizeof(value);
        }

        static std::uint32_t writeInt64(std::vector<std::uint8_t>& buffer, std::uint64_t value)
        {
            std::array<std::uint8_t, sizeof(value)> data;

            encodeBigEndian<std::uint64_t>(data.data(), value);

            buffer.insert(buffer.end(), data.begin(), data.end());

            return sizeof(value);
        }

        static std::uint32_t writeFloat(std::vector<std::uint8_t>& buffer, float value)
        {
            buffer.insert(buffer.end(),
                          reinterpret_cast<const std::uint8_t*>(&value),
                          reinterpret_cast<const std::uint8_t*>(&value) + sizeof(value));

            return sizeof(float);
        }

        static std::uint32_t writeDouble(std::vector<std::uint8_t>& buffer, double value)
        {
            buffer.insert(buffer.end(),
                          reinterpret_cast<const std::uint8_t*>(&value),
                          reinterpret_cast<const std::uint8_t*>(&value) + sizeof(value));

            return sizeof(double);
        }

        static std::uint32_t writeString(std::vector<std::uint8_t>& buffer,
                                         const std::string& value)
        {
            std::uint8_t lengthData[sizeof(std::uint16_t)];

            encodeBigEndian<std::uint16_t>(lengthData, static_cast<std::uint16_t>(value.length()));

            buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

            std::uint32_t size = sizeof(lengthData);

            buffer.insert(buffer.end(),
                          reinterpret_cast<const std::uint8_t*>(value.data()),
                          reinterpret_cast<const std::uint8_t*>(value.data()) + value.length());
            size += static_cast<std::uint32_t>(value.length());

            return size;
        }

        static std::uint32_t writeLongString(std::vector<std::uint8_t>& buffer,
                                             const std::string& value)
        {
            std::uint8_t lengthData[sizeof(std::uint32_t)];

            encodeBigEndian<std::uint32_t>(lengthData, static_cast<std::uint32_t>(value.length()));

            buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

            std::uint32_t size = sizeof(lengthData);

            buffer.insert(buffer.end(),
                          reinterpret_cast<const std::uint8_t*>(value.data()),
                          reinterpret_cast<const std::uint8_t*>(value.data()) + value.length());
            size += static_cast<std::uint32_t>(value.length());

            return size;
        }

        static std::uint32_t writeByteArray(std::vector<std::uint8_t>& buffer,
                                            const ByteArray& value)
        {
            std::uint8_t lengthData[sizeof(std::uint32_t)];

            encodeBigEndian<std::uint32_t>(lengthData, static_cast<std::uint32_t>(value.size()));

            buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

            std::uint32_t size = sizeof(lengthData);

            buffer.insert(buffer.end(), value.begin(), value.end());
            size += static_cast<std::uint32_t>(value.size());

            return size;
        }

        static std::uint32_t writeObject(std::vector<std::uint8_t>& buffer,
                                         const Object& value)
        {
            std::uint8_t lengthData[sizeof(std::uint32_t)];

            encodeBigEndian<std::uint32_t>(lengthData, static_cast<std::uint32_t>(value.size()));

            buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

            std::uint32_t size = sizeof(lengthData);

            for (const auto& [key, entryValue] : value)
            {
                std::uint8_t keyData[sizeof(std::uint32_t)];

                encodeBigEndian<std::uint32_t>(keyData, key);

                buffer.insert(buffer.end(), std::begin(keyData), std::end(keyData));

                size += sizeof(keyData);

                size += entryValue.encode(buffer);
            }

            return size;
        }

        static std::uint32_t writeArray(std::vector<std::uint8_t>& buffer,
                                        const Array& value)
        {
            std::uint8_t lengthData[sizeof(std::uint32_t)];

            encodeBigEndian<std::uint32_t>(lengthData, static_cast<std::uint32_t>(value.size()));

            buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

            std::uint32_t size = sizeof(lengthData);

            for (const auto& i : value)
                size += i.encode(buffer);

            return size;
        }

        static std::uint32_t writeDictionary(std::vector<std::uint8_t>& buffer,
                                             const Dictionary& value)
        {
            std::uint8_t sizeData[sizeof(std::uint32_t)];

            encodeBigEndian<std::uint32_t>(sizeData, static_cast<std::uint32_t>(value.size()));

            buffer.insert(buffer.end(), std::begin(sizeData), std::end(sizeData));

            std::uint32_t size = sizeof(sizeData);

            for (const auto& [key, entryValue] : value)
            {
                std::uint8_t lengthData[sizeof(std::uint16_t)];

                encodeBigEndian<std::uint16_t>(lengthData, static_cast<std::uint16_t>(key.length()));

                buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

                size += sizeof(lengthData);

                buffer.insert(buffer.end(),
                              reinterpret_cast<const std::uint8_t*>(key.data()),
                              reinterpret_cast<const std::uint8_t*>(key.data()) + key.length());
                size += static_cast<std::uint32_t>(key.length());

                size += entryValue.encode(buffer);
            }

            return size;
        }

        Type type = Type::object;
        union
        {
            std::uint64_t intValue = 0;
            double doubleValue;
        };
        std::string stringValue;
        ByteArray byteArrayValue;
        Object objectValue;
        Array arrayValue;
        Dictionary dictionaryValue;
    };
}

#endif // OUZEL_FORMATS_OBF_HPP
