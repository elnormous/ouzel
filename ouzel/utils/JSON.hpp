// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <map>
#include <string>
#include <type_traits>
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
            using Array = std::vector<Value>;
            using Object = std::map<std::string, Value>;

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
            Value(double value): type(Type::NUMBER), doubleValue(value) {}
            Value(const std::string& value): type(Type::STRING), stringValue(value) {}
            Value(bool value): type(Type::BOOLEAN), boolValue(value) {}
            Value(std::nullptr_t): type(Type::OBJECT), nullValue(true) {}
            Value(const Array& value): type(Type::ARRAY), arrayValue(value) {}
            Value(const Object& value): type(Type::OBJECT), objectValue(value) {}

            inline Value& operator=(Type newType)
            {
                type = newType;
                return *this;
            }

            inline Value& operator=(double value)
            {
                type = Type::NUMBER;
                doubleValue = value;
                return *this;
            }

            inline Value& operator=(int32_t value)
            {
                type = Type::NUMBER;
                doubleValue = static_cast<double>(value);
                return *this;
            }

            inline Value& operator=(uint32_t value)
            {
                type = Type::NUMBER;
                doubleValue = static_cast<double>(value);
                return *this;
            }

            inline Value& operator=(int64_t value)
            {
                type = Type::NUMBER;
                doubleValue = static_cast<double>(value);
                return *this;
            }

            inline Value& operator=(uint64_t value)
            {
                type = Type::NUMBER;
                doubleValue = static_cast<double>(value);
                return *this;
            }

            inline Value& operator=(const std::string& value)
            {
                type = Type::STRING;
                stringValue = value;
                return *this;
            }

            inline Value& operator=(bool value)
            {
                type = Type::BOOLEAN;
                boolValue = value;
                return *this;
            }

            inline Value& operator=(std::nullptr_t)
            {
                type = Type::OBJECT;
                nullValue = true;
                objectValue.clear();
                return *this;
            }

            inline Value& operator=(const Array& value)
            {
                type = Type::ARRAY;
                arrayValue = value;
                return *this;
            }

            inline Value& operator=(const Object& value)
            {
                type = Type::OBJECT;
                objectValue = value;
                nullValue = false;
                return *this;
            }

            inline Type getType() const { return type; }

            template<typename T, typename std::enable_if<std::is_same<T, std::string>::value, int>::type = 0>
            const std::string& as() const
            {
                if (type == Type::STRING) return stringValue;
                else throw DataError("Invalid value type");
            }

            template<typename T, typename std::enable_if<std::is_same<T, const char*>::value, int>::type = 0>
            const char* as() const
            {
                if (type == Type::STRING) return stringValue.c_str();
                else throw DataError("Invalid value type");
            }

            template<typename T, typename std::enable_if<std::is_arithmetic<T>::value, int>::type = 0>
            T as() const
            {
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::NUMBER) return static_cast<T>(doubleValue);
                else throw DataError("Invalid value type");
            }

            template<typename T, typename std::enable_if<std::is_same<T, Object>::value, int>::type = 0>
            inline Object& as()
            {
                type = Type::OBJECT;
                return objectValue;
            }

            template<typename T, typename std::enable_if<std::is_same<T, Object>::value, int>::type = 0>
            inline const Object& as() const
            {
                if (type == Type::OBJECT) return objectValue;
                else throw DataError("Invalid value type");
            }

            template<typename T, typename std::enable_if<std::is_same<T, Array>::value, int>::type = 0>
            inline Array& as()
            {
                type = Type::ARRAY;
                return arrayValue;
            }

            template<typename T, typename std::enable_if<std::is_same<T, Array>::value, int>::type = 0>
            inline const Array& as() const
            {
                if (type == Type::ARRAY) return arrayValue;
                else throw DataError("Invalid value type");
            }

            inline bool isNull() const
            {
                if (type == Type::OBJECT) return nullValue;
                else throw DataError("Invalid value type");
            }

            inline bool hasMember(const std::string& member) const
            {
                if (type == Type::OBJECT)
                    return objectValue.find(member) != objectValue.end();
                else throw DataError("Invalid value type");
            }

            inline Value& operator[](const std::string& member)
            {
                type = Type::OBJECT;
                nullValue = false;
                return objectValue[member];
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

            inline Value& operator[](size_t index)
            {
                type = Type::ARRAY;
                if (index >= arrayValue.size()) arrayValue.resize(index + 1);
                return arrayValue[index];
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
            Object objectValue;
            Array arrayValue;
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
