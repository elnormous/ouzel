// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cassert>
#include <map>
#include <string>
#include <vector>

namespace ouzel
{
    namespace json
    {
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

            Value() {}
            Value(Type initType): type(initType) {}
            Value(double val): type(Type::NUMBER), doubleValue(val) {}
            Value(const std::string& str): type(Type::STRING), stringValue(str) {}
            Value(bool val): type(Type::BOOLEAN), boolValue(val) {}

            inline Value& operator=(Type newType)
            {
                type = newType;
                return *this;
            }

            inline Value& operator=(double val)
            {
                type = Type::NUMBER;
                doubleValue = val;
                return *this;
            }

            inline Value& operator=(int32_t val)
            {
                type = Type::NUMBER;
                doubleValue = static_cast<double>(val);
                return *this;
            }

            inline Value& operator=(uint32_t val)
            {
                type = Type::NUMBER;
                doubleValue = static_cast<double>(val);
                return *this;
            }

            inline Value& operator=(int64_t val)
            {
                type = Type::NUMBER;
                doubleValue = static_cast<double>(val);
                return *this;
            }

            inline Value& operator=(uint64_t val)
            {
                type = Type::NUMBER;
                doubleValue = static_cast<double>(val);
                return *this;
            }

            inline Value& operator=(const std::string& str)
            {
                type = Type::STRING;
                stringValue = str;
                return *this;
            }

            inline Value& operator=(bool val)
            {
                type = Type::BOOLEAN;
                boolValue = val;
                return *this;
            }

            inline Type getType() const { return type; }

            inline bool asBool() const
            {
                assert(type == Type::BOOLEAN || type == Type::NUMBER);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::NUMBER) return doubleValue != 0.0F;
                else return false;
            }

            inline float asFloat() const
            {
                assert(type == Type::BOOLEAN || type == Type::NUMBER);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::NUMBER) return static_cast<float>(doubleValue);
                else return 0.0F;
            }

            inline double asDouble() const
            {
                assert(type == Type::BOOLEAN || type == Type::NUMBER);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::NUMBER) return doubleValue;
                else return 0.0;
            }

            inline int32_t asInt32() const
            {
                assert(type == Type::BOOLEAN || type == Type::NUMBER);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::NUMBER) return static_cast<int32_t>(doubleValue);
                else return 0;
            }

            inline uint32_t asUInt32() const
            {
                assert(type == Type::BOOLEAN || type == Type::NUMBER);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::NUMBER) return static_cast<uint32_t>(doubleValue);
                else return 0;
            }

            inline int64_t asInt64() const
            {
                assert(type == Type::BOOLEAN || type == Type::NUMBER);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::NUMBER) return static_cast<int64_t>(doubleValue);
                else return 0;
            }

            inline uint64_t asUInt64() const
            {
                assert(type == Type::BOOLEAN || type == Type::NUMBER);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::NUMBER) return static_cast<uint64_t>(doubleValue);
                else return 0;
            }

            inline const std::string& asString() const
            {
                assert(type == Type::STRING);
                return stringValue;
            }

            inline bool isNull() const
            {
                assert(type == Type::OBJECT);
                return nullValue;
            }

            inline void setNull(bool val)
            {
                nullValue = val;
            }

            inline bool hasMember(const std::string& member) const
            {
                assert(type == Type::OBJECT);
                return objectValue.find(member) != objectValue.end();
            }

            inline Value& operator[](const std::string& member)
            {
                assert(type == Type::OBJECT);
                return objectValue[member];
            }

            inline Value operator[](const std::string& member) const
            {
                assert(type == Type::OBJECT);
                auto i = objectValue.find(member);
                if (i != objectValue.end()) return i->second;
                else return Value();
            }

            inline const std::map<std::string, Value>& asMap() const
            {
                assert(type == Type::OBJECT);
                return objectValue;
            }

            inline Value& operator[](size_t index)
            {
                assert(type == Type::ARRAY);
                if (index >= arrayValue.size()) arrayValue.resize(index + 1);
                return arrayValue[index];
            }

            inline Value operator[](size_t index) const
            {
                assert(type == Type::ARRAY);
                if (index < arrayValue.size()) return arrayValue[index];
                else return Value();
            }

            inline std::vector<Value>& asArray()
            {
                assert(type == Type::ARRAY);
                return arrayValue;
            }

            inline const std::vector<Value>& asArray() const
            {
                assert(type == Type::ARRAY);
                return arrayValue;
            }

            inline size_t getSize() const
            {
                assert(type == Type::ARRAY);
                return arrayValue.size();
            }

        protected:
            void parseValue(const std::vector<Token>& tokens,
                            std::vector<Token>::const_iterator& iterator);
            void parseObject(const std::vector<Token>& tokens,
                             std::vector<Token>::const_iterator& iterator);
            void parseArray(const std::vector<Token>& tokens,
                            std::vector<Token>::const_iterator& iterator);
            void encodeValue(std::vector<uint8_t>& data) const;

            Type type = Type::NONE;
            bool boolValue = false;
            bool nullValue = false;
            double doubleValue = 0.0;
            std::map<std::string, Value> objectValue;
            std::vector<Value> arrayValue;
            std::string stringValue;
        };

        class Data: public Value
        {
        public:
            Data();
            Data(const std::vector<uint8_t>& data);

            std::vector<uint8_t> encode() const;

            bool hasBOM() const { return bom; }
            void setBOM(bool newBOM) { bom = newBOM; }

        private:
            bool bom = false;
        };
    } // namespace json
} // namespace ouzel
