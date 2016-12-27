// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cassert>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace ouzel
{
    namespace obf
    {
        class Value
        {
        public:
            enum class Type: uint8_t
            {
                NONE,
                INT8,
                INT16,
                INT32,
                INT64,
                FLOAT,
                DOUBLE,
                STRING,
                LONG_STRING,
                BYTE_ARRAY,
                OBJECT,
                ARRAY
            };

            Value() {}
            Value(Type aType): type(aType) {}
            Value(int8_t value): type(Type::INT8), intValue(value) {}
            Value(int16_t value): type(Type::INT16), intValue(value) {}
            Value(int32_t value): type(Type::INT32), intValue(value) {}
            Value(int64_t value): type(Type::INT64), intValue(value) {}
            Value(float value): type(Type::FLOAT), doubleValue(value) {}
            Value(double value): type(Type::DOUBLE), doubleValue(value) {}
            Value(const std::string& value):
                stringValue(value)
            {
                if (value.length() <= UINT16_MAX)
                {
                    type = Type::STRING;
                }
                else
                {
                    type = Type::LONG_STRING;
                }
            }
            Value(const std::vector<uint8_t>& value): type(Type::BYTE_ARRAY), byteArrayValue(value) {}
            Value(const std::map<uint32_t, Value>& value): type(Type::OBJECT), mapValue(value) {}
            Value(const std::vector<Value>& value): type(Type::ARRAY), vectorValue(value) {}

            Value& operator=(Type newType)
            {
                type = newType;

                return *this;
            }

            Value& operator=(float value)
            {
                type = Type::FLOAT;
                doubleValue = value;

                return *this;
            }

            Value& operator=(double value)
            {
                type = Type::DOUBLE;
                doubleValue = value;

                return *this;
            }

            Value& operator=(const std::string& value)
            {
                stringValue = value;
                if (value.length() <= UINT16_MAX)
                {
                    type = Type::STRING;
                }
                else
                {
                    type = Type::LONG_STRING;
                }
                return *this;
            }

            Value& operator=(const std::vector<uint8_t>& value)
            {
                type = Type::BYTE_ARRAY;
                byteArrayValue = value;

                return *this;
            }

            Value& operator=(const std::map<uint32_t, Value>& value)
            {
                type = Type::OBJECT;
                mapValue = value;

                return *this;
            }

            Value& operator=(const std::vector<Value>& value)
            {
                type = Type::ARRAY;
                vectorValue = value;

                return *this;
            }

            Type getType() const { return type; }

            uint32_t decode(const std::vector<uint8_t>& buffer, uint32_t offset = 0);
            uint32_t encode(std::vector<uint8_t>& buffer) const;

            int8_t asInt8() const
            {
                assert(type == Type::INT8 || type == Type::INT16 || type == Type::INT32 || type == Type::INT64);

                return static_cast<int8_t>(intValue);
            }

            uint8_t asUInt8() const
            {
                assert(type == Type::INT8 || type == Type::INT16 || type == Type::INT32 || type == Type::INT64);

                return static_cast<uint8_t>(intValue);
            }

            int16_t asInt16() const
            {
                assert(type == Type::INT8 || type == Type::INT16 || type == Type::INT32 || type == Type::INT64);

                return static_cast<int16_t>(intValue);
            }

            uint16_t asUInt16() const
            {
                assert(type == Type::INT8 || type == Type::INT16 || type == Type::INT32 || type == Type::INT64);

                return static_cast<uint16_t>(intValue);
            }

            int32_t asInt32() const
            {
                assert(type == Type::INT8 || type == Type::INT16 || type == Type::INT32 || type == Type::INT64);

                return static_cast<int32_t>(intValue);
            }

            uint32_t asUInt32() const
            {
                assert(type == Type::INT8 || type == Type::INT16 || type == Type::INT32 || type == Type::INT64);

                return static_cast<uint32_t>(intValue);
            }

            int64_t asInt64() const
            {
                assert(type == Type::INT8 || type == Type::INT16 || type == Type::INT32 || type == Type::INT64);

                return intValue;
            }

            uint64_t asUInt64() const
            {
                assert(type == Type::INT8 || type == Type::INT16 || type == Type::INT32 || type == Type::INT64);

                return static_cast<uint64_t>(intValue);
            }

            float asFloat() const
            {
                assert(type == Type::FLOAT || type == Type::DOUBLE);

                return static_cast<float>(doubleValue);
            }

            double asDouble() const
            {
                assert(type == Type::FLOAT || type == Type::DOUBLE);

                return doubleValue;
            }

            const std::string& asString() const
            {
                assert(type == Type::STRING || type == Type::LONG_STRING);

                return stringValue;
            }

            const std::vector<Value>& asVector() const
            {
                assert(type == Type::ARRAY);

                return vectorValue;
            }

            const std::map<uint32_t, Value>& asMap() const
            {
                assert(type == Type::OBJECT);

                return mapValue;
            }

            const std::vector<uint8_t>& asByteArray() const
            {
                assert(type == Type::BYTE_ARRAY);

                return byteArrayValue;
            }

            uint32_t getSize() const
            {
                assert(type == Type::ARRAY);

                return static_cast<uint32_t>(vectorValue.size());
            }

            Value operator[](uint32_t key) const
            {
                assert(type == Type::OBJECT || type == Type::ARRAY);

                if (type == Type::OBJECT)
                {
                    auto i = mapValue.find(key);

                    if (i != mapValue.end())
                    {
                        return i->second;
                    }
                }
                else if (type == Type::ARRAY)
                {
                    if (key < vectorValue.size())
                    {
                        return vectorValue[key];
                    }
                }

                return Value();
            }

            Value& operator[](uint32_t key)
            {
                assert(type == Type::OBJECT || type == Type::ARRAY);

                if (type == Type::OBJECT)
                {
                    return mapValue[key];
                }
                else
                {
                    type = Type::ARRAY;

                    return vectorValue[key];
                }
            }

            bool hasElement(uint32_t key) const
            {
                assert(type == Type::OBJECT || type == Type::ARRAY);

                if (type == Type::OBJECT)
                {
                    return mapValue.find(key) != mapValue.end();
                }
                else if (type == Type::ARRAY)
                {
                    return key < vectorValue.size();
                }
                else
                {
                    return false;
                }
            }

            void append(const Value& node)
            {
                vectorValue.push_back(node);
            }

        private:
            Type type = Type::NONE;
            int64_t intValue;
            double doubleValue;
            std::string stringValue;
            std::vector<Value> vectorValue;
            std::map<uint32_t, Value> mapValue;
            std::vector<uint8_t> byteArrayValue;
        };
    } // namespace obf
} // namespace ouzel
