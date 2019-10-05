// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <cstring>
#include <stdexcept>
#include "Obf.hpp"
#include "Utils.hpp"

namespace ouzel
{
    namespace obf
    {
        namespace
        {
            // reading
            uint32_t readInt8(const std::vector<uint8_t>& buffer, uint32_t offset, uint8_t& result)
            {
                if (buffer.size() - offset < sizeof(result))
                    throw std::runtime_error("Not enough data");

                memcpy(&result, buffer.data() + offset, sizeof(result));

                return sizeof(result);
            }

            uint32_t readInt16(const std::vector<uint8_t>& buffer, uint32_t offset, uint16_t& result)
            {
                if (buffer.size() - offset < sizeof(result))
                    throw std::runtime_error("Not enough data");

                result = decodeBigEndian<uint16_t>(buffer.data() + offset);

                return sizeof(result);
            }

            uint32_t readInt32(const std::vector<uint8_t>& buffer, uint32_t offset, uint32_t& result)
            {
                if (buffer.size() - offset < sizeof(result))
                    throw std::runtime_error("Not enough data");

                result = decodeBigEndian<uint32_t>(buffer.data() + offset);

                return sizeof(result);
            }

            uint32_t readInt64(const std::vector<uint8_t>& buffer, uint32_t offset, uint64_t& result)
            {
                if (buffer.size() - offset < sizeof(result))
                    throw std::runtime_error("Not enough data");

                result = decodeBigEndian<uint64_t>(buffer.data() + offset);

                return sizeof(result);
            }

            uint32_t readFloat(const std::vector<uint8_t>& buffer, uint32_t offset, float& result)
            {
                if (buffer.size() - offset < sizeof(float))
                    throw std::runtime_error("Not enough data");

                memcpy(&result, buffer.data() + offset, sizeof(result));

                return sizeof(result);
            }

            uint32_t readDouble(const std::vector<uint8_t>& buffer, uint32_t offset, double& result)
            {
                if (buffer.size() - offset < sizeof(double))
                    throw std::runtime_error("Not enough data");

                memcpy(&result, buffer.data() + offset, sizeof(result));

                return sizeof(result);
            }

            uint32_t readString(const std::vector<uint8_t>& buffer, uint32_t offset, std::string& result)
            {
                uint32_t originalOffset = offset;

                if (buffer.size() - offset < sizeof(uint16_t))
                    throw std::runtime_error("Not enough data");

                uint16_t length = decodeBigEndian<uint16_t>(buffer.data() + offset);

                offset += sizeof(length);

                if (buffer.size() - offset < length)
                    throw std::runtime_error("Not enough data");

                result.assign(reinterpret_cast<const char*>(buffer.data() + offset), length);
                offset += length;

                return offset - originalOffset;
            }

            uint32_t readLongString(const std::vector<uint8_t>& buffer, uint32_t offset, std::string& result)
            {
                uint32_t originalOffset = offset;

                if (buffer.size() - offset < sizeof(uint32_t))
                    throw std::runtime_error("Not enough data");

                uint32_t length = decodeBigEndian<uint32_t>(buffer.data() + offset);

                offset += sizeof(length);

                if (buffer.size() - offset < length)
                    throw std::runtime_error("Not enough data");

                result.assign(reinterpret_cast<const char*>(buffer.data() + offset), length);
                offset += length;

                return offset - originalOffset;
            }

            uint32_t readByteArray(const std::vector<uint8_t>& buffer, uint32_t offset, std::vector<uint8_t>& result)
            {
                uint32_t originalOffset = offset;

                if (buffer.size() - offset < sizeof(uint32_t))
                    throw std::runtime_error("Not enough data");

                uint32_t length = decodeBigEndian<uint32_t>(buffer.data() + offset);

                offset += sizeof(length);

                if (buffer.size() - offset < length)
                    throw std::runtime_error("Not enough data");

                result.assign(reinterpret_cast<const uint8_t*>(buffer.data() + offset),
                              reinterpret_cast<const uint8_t*>(buffer.data() + offset) + length);
                offset += length;

                return offset - originalOffset;
            }

            uint32_t readObject(const std::vector<uint8_t>& buffer, uint32_t offset, std::map<uint32_t, Value>& result)
            {
                uint32_t originalOffset = offset;

                if (buffer.size() - offset < sizeof(uint32_t))
                    throw std::runtime_error("Not enough data");

                uint32_t count = decodeBigEndian<uint32_t>(buffer.data() + offset);

                offset += sizeof(count);

                for (uint32_t i = 0; i < count; ++i)
                {
                    if (buffer.size() - offset < sizeof(uint32_t))
                        throw std::runtime_error("Not enough data");

                    uint32_t key = decodeBigEndian<uint32_t>(buffer.data() + offset);

                    offset += sizeof(key);

                    Value node;

                    uint32_t ret = node.decode(buffer, offset);

                    offset += ret;

                    result[static_cast<uint32_t>(key)] = node;
                }

                return offset - originalOffset;
            }

            uint32_t readArray(const std::vector<uint8_t>& buffer, uint32_t offset, std::vector<Value>& result)
            {
                uint32_t originalOffset = offset;

                if (buffer.size() - offset < sizeof(uint32_t))
                    throw std::runtime_error("Not enough data");

                uint32_t count = decodeBigEndian<uint32_t>(buffer.data() + offset);

                offset += sizeof(count);

                for (uint32_t i = 0; i < count; ++i)
                {
                    Value node;
                    uint32_t ret = node.decode(buffer, offset);

                    offset += ret;

                    result.push_back(node);
                }

                return offset - originalOffset;
            }

            uint32_t readDictionary(const std::vector<uint8_t>& buffer, uint32_t offset, std::map<std::string, Value>& result)
            {
                uint32_t originalOffset = offset;

                if (buffer.size() - offset < sizeof(uint32_t))
                    throw std::runtime_error("Not enough data");

                uint32_t count = decodeBigEndian<uint32_t>(buffer.data() + offset);

                offset += sizeof(count);

                for (uint32_t i = 0; i < count; ++i)
                {
                    if (buffer.size() - offset < sizeof(uint16_t))
                        throw std::runtime_error("Not enough data");

                    uint16_t length = decodeBigEndian<uint16_t>(buffer.data() + offset);

                    offset += sizeof(length);

                    if (buffer.size() - offset < length)
                        throw std::runtime_error("Not enough data");

                    std::string key(reinterpret_cast<const char*>(buffer.data() + offset), length);
                    offset += length;

                    Value node;

                    uint32_t ret = node.decode(buffer, offset);

                    offset += ret;

                    result[key] = node;
                }

                return offset - originalOffset;
            }

            // writing
            uint32_t writeInt8(std::vector<uint8_t>& buffer, uint8_t value)
            {
                buffer.push_back(value);

                return sizeof(value);
            }

            uint32_t writeInt16(std::vector<uint8_t>& buffer, uint16_t value)
            {
                uint8_t data[sizeof(value)];

                encodeBigEndian<uint16_t>(data, value);

                buffer.insert(buffer.end(), std::begin(data), std::end(data));

                return sizeof(value);
            }

            uint32_t writeInt32(std::vector<uint8_t>& buffer, uint32_t value)
            {
                uint8_t data[sizeof(value)];

                encodeBigEndian<uint32_t>(data, value);

                buffer.insert(buffer.end(), std::begin(data), std::end(data));

                return sizeof(value);
            }

            uint32_t writeInt64(std::vector<uint8_t>& buffer, uint64_t value)
            {
                uint8_t data[sizeof(value)];

                encodeBigEndian<uint64_t>(data, value);

                buffer.insert(buffer.end(), std::begin(data), std::end(data));

                return sizeof(value);
            }

            uint32_t writeFloat(std::vector<uint8_t>& buffer, float value)
            {
                buffer.insert(buffer.end(),
                              reinterpret_cast<const uint8_t*>(&value),
                              reinterpret_cast<const uint8_t*>(&value) + sizeof(value));

                return sizeof(float);
            }

            uint32_t writeDouble(std::vector<uint8_t>& buffer, double value)
            {
                buffer.insert(buffer.end(),
                              reinterpret_cast<const uint8_t*>(&value),
                              reinterpret_cast<const uint8_t*>(&value) + sizeof(value));

                return sizeof(double);
            }

            uint32_t writeString(std::vector<uint8_t>& buffer, const std::string& value)
            {
                uint8_t lengthData[sizeof(uint16_t)];

                encodeBigEndian<uint16_t>(lengthData, static_cast<uint16_t>(value.length()));

                buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

                uint32_t size = sizeof(lengthData);

                buffer.insert(buffer.end(),
                              reinterpret_cast<const uint8_t*>(value.data()),
                              reinterpret_cast<const uint8_t*>(value.data()) + value.length());
                size += static_cast<uint32_t>(value.length());

                return size;
            }

            uint32_t writeLongString(std::vector<uint8_t>& buffer, const std::string& value)
            {
                uint8_t lengthData[sizeof(uint32_t)];

                encodeBigEndian<uint32_t>(lengthData, static_cast<uint32_t>(value.length()));

                buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

                uint32_t size = sizeof(lengthData);

                buffer.insert(buffer.end(),
                              reinterpret_cast<const uint8_t*>(value.data()),
                              reinterpret_cast<const uint8_t*>(value.data()) + value.length());
                size += static_cast<uint32_t>(value.length());

                return size;
            }

            uint32_t writeByteArray(std::vector<uint8_t>& buffer, const std::vector<uint8_t>& value)
            {
                uint8_t lengthData[sizeof(uint32_t)];

                encodeBigEndian<uint32_t>(lengthData, static_cast<uint32_t>(value.size()));

                buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

                uint32_t size = sizeof(lengthData);

                buffer.insert(buffer.end(), value.begin(), value.end());
                size += static_cast<uint32_t>(value.size());

                return size;
            }

            uint32_t writeObject(std::vector<uint8_t>& buffer, const std::map<uint32_t, Value>& value)
            {
                uint8_t lengthData[sizeof(uint32_t)];

                encodeBigEndian<uint32_t>(lengthData, static_cast<uint32_t>(value.size()));

                buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

                uint32_t size = sizeof(lengthData);

                for (const auto& i : value)
                {
                    uint8_t keyData[sizeof(uint32_t)];

                    encodeBigEndian<uint32_t>(keyData, i.first);

                    buffer.insert(buffer.end(), std::begin(keyData), std::end(keyData));

                    size += sizeof(keyData);

                    size += i.second.encode(buffer);
                }

                return size;
            }

            uint32_t writeArray(std::vector<uint8_t>& buffer, const std::vector<Value>& value)
            {
                uint8_t lengthData[sizeof(uint32_t)];

                encodeBigEndian<uint32_t>(lengthData, static_cast<uint32_t>(value.size()));

                buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

                uint32_t size = sizeof(lengthData);

                for (const auto& i : value)
                    size += i.encode(buffer);

                return size;
            }

            uint32_t writeDictionary(std::vector<uint8_t>& buffer, const std::map<std::string, Value>& value)
            {
                uint8_t sizeData[sizeof(uint32_t)];

                encodeBigEndian<uint32_t>(sizeData, static_cast<uint32_t>(value.size()));

                buffer.insert(buffer.end(), std::begin(sizeData), std::end(sizeData));

                uint32_t size = sizeof(sizeData);

                for (const auto& i : value)
                {
                    uint8_t lengthData[sizeof(uint16_t)];

                    encodeBigEndian<uint16_t>(lengthData, static_cast<uint16_t>(i.first.length()));

                    buffer.insert(buffer.end(), std::begin(lengthData), std::end(lengthData));

                    size += sizeof(lengthData);

                    buffer.insert(buffer.end(),
                                  reinterpret_cast<const uint8_t*>(i.first.data()),
                                  reinterpret_cast<const uint8_t*>(i.first.data()) + i.first.length());
                    size += static_cast<uint32_t>(i.first.length());

                    size += i.second.encode(buffer);
                }

                return size;
            }
        }

        uint32_t Value::decode(const std::vector<uint8_t>& buffer, uint32_t offset)
        {
            uint32_t originalOffset = offset;

            if (buffer.size() - offset < 1)
                throw std::runtime_error("Not enough data");

            Marker marker;
            memcpy(&marker, buffer.data() + offset, sizeof(marker));
            offset += 1;

            uint32_t ret = 0;

            switch (marker)
            {
                case Marker::Int8:
                {
                    type = Type::Int;

                    uint8_t int8Value;
                    ret = readInt8(buffer, offset, int8Value);

                    intValue = int8Value;
                    break;
                }
                case Marker::Int16:
                {
                    type = Type::Int;

                    uint16_t int16Value;
                    ret = readInt16(buffer, offset, int16Value);

                    intValue = int16Value;
                    break;
                }
                case Marker::Int32:
                {
                    type = Type::Int;

                    uint32_t int32Value;
                    ret = readInt32(buffer, offset, int32Value);

                    intValue = int32Value;
                    break;
                }
                case Marker::Int64:
                {
                    type = Type::Int;

                    ret = readInt64(buffer, offset, intValue);
                    break;
                }
                case Marker::Float:
                {
                    type = Type::Float;

                    float floatValue;
                    ret = readFloat(buffer, offset, floatValue);

                    doubleValue = floatValue;
                    break;
                }
                case Marker::Double:
                {
                    type = Type::Double;

                    ret = readDouble(buffer, offset, doubleValue);
                    break;
                }
                case Marker::String:
                {
                    type = Type::String;

                    ret = readString(buffer, offset, stringValue);
                    break;
                }
                case Marker::LongString:
                {
                    type = Type::String;

                    ret = readLongString(buffer, offset, stringValue);
                    break;
                }
                case Marker::ByteArray:
                {
                    type = Type::ByteArray;

                    ret = readByteArray(buffer, offset, byteArrayValue);
                    break;
                }
                case Marker::Object:
                {
                    type = Type::Object;

                    ret = readObject(buffer, offset, objectValue);
                    break;
                }
                case Marker::Array:
                {
                    type = Type::Array;

                    ret = readArray(buffer, offset, arrayValue);
                    break;
                }
                case Marker::Dictionary:
                {
                    type = Type::Dictionary;

                    ret = readDictionary(buffer, offset, dictionaryValue);
                    break;
                }
                default:
                    throw std::runtime_error("Unsupported marker");
            }

            offset += ret;

            return offset - originalOffset;
        }

        uint32_t Value::encode(std::vector<uint8_t>& buffer) const
        {
            uint32_t size = 0;

            uint32_t ret = 0;

            switch (type)
            {
                case Type::Int:
                {
                    if (intValue > std::numeric_limits<uint32_t>::max())
                    {
                        buffer.push_back(static_cast<uint8_t>(Marker::Int64));
                        size += 1;
                        ret = writeInt64(buffer, intValue);
                    }
                    else if (intValue > std::numeric_limits<uint16_t>::max())
                    {
                        buffer.push_back(static_cast<uint8_t>(Marker::Int32));
                        size += 1;
                        ret = writeInt32(buffer, static_cast<uint32_t>(intValue));
                    }
                    else if (intValue > std::numeric_limits<uint8_t>::max())
                    {
                        buffer.push_back(static_cast<uint8_t>(Marker::Int16));
                        size += 1;
                        ret = writeInt16(buffer, static_cast<uint16_t>(intValue));
                    }
                    else
                    {
                        buffer.push_back(static_cast<uint8_t>(Marker::Int8));
                        size += 1;
                        ret = writeInt8(buffer, static_cast<uint8_t>(intValue));
                    }
                    break;
                }
                case Type::Float:
                {
                    buffer.push_back(static_cast<uint8_t>(Marker::Float));
                    size += 1;
                    ret = writeFloat(buffer, static_cast<float>(doubleValue));
                    break;
                }
                case Type::Double:
                {
                    buffer.push_back(static_cast<uint8_t>(Marker::Double));
                    size += 1;
                    ret = writeDouble(buffer, doubleValue);
                    break;
                }
                case Type::String:
                {
                    if (stringValue.length() > std::numeric_limits<uint16_t>::max())
                    {
                        buffer.push_back(static_cast<uint8_t>(Marker::LongString));
                        size += 1;
                        ret = writeLongString(buffer, stringValue);
                    }
                    else
                    {
                        buffer.push_back(static_cast<uint8_t>(Marker::String));
                        size += 1;
                        ret = writeString(buffer, stringValue);
                    }
                    break;
                }
                case Type::ByteArray:
                {
                    buffer.push_back(static_cast<uint8_t>(Marker::ByteArray));
                    size += 1;
                    ret = writeByteArray(buffer, byteArrayValue);
                    break;
                }
                case Type::Object:
                {
                    buffer.push_back(static_cast<uint8_t>(Marker::Object));
                    size += 1;
                    ret = writeObject(buffer, objectValue);
                    break;
                }
                case Type::Array:
                {
                    buffer.push_back(static_cast<uint8_t>(Marker::Array));
                    size += 1;
                    ret = writeArray(buffer, arrayValue);
                    break;
                }
                case Type::Dictionary:
                {
                    buffer.push_back(static_cast<uint8_t>(Marker::Dictionary));
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
    } // namespace obf
} // namespace ouzel
