// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <map>
#include <string>
#include <vector>
#include "utils/Errors.hpp"

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

            template<class T> T as() const;

            inline bool isNull() const
            {
                if (type == Type::OBJECT) return nullValue;
                else throw DataError("Invalid value type");
            }

            inline void setNull(bool val)
            {
                type = Type::OBJECT;
                nullValue = val;
            }

            inline bool hasMember(const std::string& member) const
            {
                if (type == Type::OBJECT)
                    return objectValue.find(member) != objectValue.end();
                else throw DataError("Invalid value type");
            }

            inline Value& operator[](const std::string& member)
            {
                if (type == Type::OBJECT) return objectValue[member];
                else throw DataError("Invalid value type");
            }

            inline Value operator[](const std::string& member) const
            {
                if (type == Type::OBJECT)
                {
                    auto i = objectValue.find(member);
                    if (i != objectValue.end()) return i->second;
                    else return Value();
                }
                else throw DataError("Invalid value type");
            }

            inline const std::map<std::string, Value>& asMap() const
            {
                if (type == Type::OBJECT) return objectValue;
                else throw DataError("Invalid value type");
            }

            inline Value& operator[](size_t index)
            {
                if (type == Type::ARRAY)
                {
                    if (index >= arrayValue.size()) arrayValue.resize(index + 1);
                    return arrayValue[index];
                }
                else throw DataError("Invalid value type");
            }

            inline Value operator[](size_t index) const
            {
                if (type == Type::ARRAY)
                {
                    if (index < arrayValue.size()) return arrayValue[index];
                    else return Value();
                }
                else throw DataError("Invalid value type");
            }

            inline std::vector<Value>& asArray()
            {
                if (type == Type::ARRAY) return arrayValue;
                else throw DataError("Invalid value type");
            }

            inline const std::vector<Value>& asArray() const
            {
                if (type == Type::ARRAY) return arrayValue;
                else throw DataError("Invalid value type");
            }

            inline size_t getSize() const
            {
                if (type == Type::ARRAY) return arrayValue.size();
                else throw DataError("Invalid value type");
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

        template<> inline bool Value::as<bool>() const
        {
            if (type == Type::BOOLEAN) return boolValue;
            else if (type == Type::NUMBER) return static_cast<bool>(doubleValue);
            else throw DataError("Invalid value type");
        }

        template<> inline float Value::as<float>() const
        {
            if (type == Type::BOOLEAN) return boolValue;
            else if (type == Type::NUMBER) return static_cast<float>(doubleValue);
            else throw DataError("Invalid value type");
        }

        template<> inline double Value::as<double>() const
        {
            if (type == Type::BOOLEAN) return boolValue;
            else if (type == Type::NUMBER) return doubleValue;
            else throw DataError("Invalid value type");
        }

        template<> inline int32_t Value::as<int32_t>() const
        {
            if (type == Type::BOOLEAN) return boolValue;
            else if (type == Type::NUMBER) return static_cast<int32_t>(doubleValue);
            else throw DataError("Invalid value type");
        }

        template<> inline uint32_t Value::as<uint32_t>() const
        {
            if (type == Type::BOOLEAN) return boolValue;
            else if (type == Type::NUMBER) return static_cast<uint32_t>(doubleValue);
            else throw DataError("Invalid value type");
        }

        template<> inline int64_t Value::as<int64_t>() const
        {
            if (type == Type::BOOLEAN) return boolValue;
            else if (type == Type::NUMBER) return static_cast<int64_t>(doubleValue);
            else throw DataError("Invalid value type");
        }

        template<> inline uint64_t Value::as<uint64_t>() const
        {
            if (type == Type::BOOLEAN) return boolValue;
            else if (type == Type::NUMBER) return static_cast<uint64_t>(doubleValue);
            else throw DataError("Invalid value type");
        }

        template<> inline std::string Value::as<std::string>() const
        {
            if (type == Type::STRING) return stringValue;
            else throw DataError("Invalid value type");
        }
    } // namespace json
} // namespace ouzel
