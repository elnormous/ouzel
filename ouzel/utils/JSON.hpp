// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cassert>
#include <map>
#include <string>
#include <vector>

namespace ouzel
{
    namespace json
    {
        class Value
        {
        public:
            enum class Type
            {
                NONE,
                NUMBER,
                STRING,
                OBJECT,
                ARRAY,
                BOOLEAN
            };

            Value();
            Value(const std::string& filename);
            Value(const std::vector<uint8_t>& data);

            bool init(const std::string& filename);
            bool init(const std::vector<uint8_t>& data);

            bool asBool() const
            {
                assert(type == Type::BOOLEAN || type == Type::NUMBER);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::NUMBER) return doubleValue != 0.0f;
                else return false;
            }

            float asFloat() const
            {
                assert(type == Type::BOOLEAN || type == Type::NUMBER);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::NUMBER) return static_cast<float>(doubleValue);
                else return 0.0f;
            }

            double asDouble() const
            {
                assert(type == Type::BOOLEAN || type == Type::NUMBER);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::NUMBER) return doubleValue;
                else return 0.0;
            }

            int32_t asInt32() const
            {
                assert(type == Type::BOOLEAN || type == Type::NUMBER);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::NUMBER) return static_cast<int32_t>(doubleValue);
                else return 0;
            }

            uint32_t asUInt32() const
            {
                assert(type == Type::BOOLEAN || type == Type::NUMBER);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::NUMBER) return static_cast<uint32_t>(doubleValue);
                else return 0;
            }

            int64_t asInt64() const
            {
                assert(type == Type::BOOLEAN || type == Type::NUMBER);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::NUMBER) return static_cast<int64_t>(doubleValue);
                else return 0;
            }

            uint64_t asUInt64() const
            {
                assert(type == Type::BOOLEAN || type == Type::NUMBER);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::NUMBER) return static_cast<uint64_t>(doubleValue);
                else return 0;
            }

            const std::string& asString() const
            {
                assert(type == Type::STRING);
                return stringValue;
            }

            bool isNull() const
            {
                assert(type == Type::OBJECT);
                return nullValue;
            }

            bool hasMember(const std::string& member) const
            {
                assert(type == Type::OBJECT);
                return objectValue.find(member) != objectValue.end();
            }

            Value& operator[](const std::string& member)
            {
                assert(type == Type::OBJECT);
                return objectValue[member];
            }

            Value operator[](const std::string& member) const
            {
                assert(type == Type::OBJECT);
                auto i = objectValue.find(member);
                if (i != objectValue.end()) return i->second;
                else return Value();
            }

            const std::map<std::string, Value>& asMap() const
            {
                assert(type == Type::OBJECT);
                return objectValue;
            }

            Value& operator[](size_t index)
            {
                assert(type == Type::ARRAY);
                if (index >= arrayValue.size()) arrayValue.resize(index + 1);
                return arrayValue[index];
            }

            Value operator[](size_t index) const
            {
                assert(type == Type::ARRAY);
                if (index < arrayValue.size()) return arrayValue[index];
                else return Value();
            }

            const std::vector<Value>& asArray() const
            {
                assert(type == Type::ARRAY);
                return arrayValue;
            }

            size_t getSize() const
            {
                assert(type == Type::ARRAY);
                return arrayValue.size();
            }

        private:
            struct Token
            {
                enum class Type
                {
                    NONE,
                    LITERAL_NUMBER, // float
                    LITERAL_STRING, // string
                    KEYWORD_TRUE, // true
                    KEYWORD_FALSE, // false
                    KEYWORD_NULL, // null
                    LEFT_BRACE, // {
                    RIGHT_BRACE, // }
                    LEFT_BRACKET, // [
                    RIGHT_BRACKET, // ]
                    COMMA, // ,
                    COLON, // :
                    OPERATOR_MINUS // -
                };

                Type type = Type::NONE;
                std::vector<uint32_t> value;
            };

            bool tokenize(const std::vector<uint8_t>& data, std::vector<Token>& tokens);
            bool parseValue(const std::vector<Token>& tokens,
                            std::vector<Token>::iterator& iterator);
            bool parseObject(const std::vector<Token>& tokens,
                             std::vector<Token>::iterator& iterator);
            bool parseArray(const std::vector<Token>& tokens,
                            std::vector<Token>::iterator& iterator);

            Type type = Type::NONE;
            std::map<std::string, Value> objectValue;
            std::vector<Value> arrayValue;
            std::string stringValue;
            union
            {
                bool boolValue = false;
                bool nullValue;
                double doubleValue;
            };
        };
    }
}

