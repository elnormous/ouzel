// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_UTILS_JSON_HPP
#define OUZEL_UTILS_JSON_HPP

#include <cassert>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include "utils/Utf8.hpp"

namespace ouzel
{
    namespace json
    {
        struct Token final
        {
            enum class Type
            {
                NONE,
                LITERAL_INTEGER, // integer
                LITERAL_FLOAT, // float
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
                INTEGER,
                FLOAT,
                STRING,
                OBJECT,
                ARRAY,
                BOOLEAN
            };

            Value() {}

            template<typename T, typename std::enable_if<std::is_same<T, Type>::value>::type* = nullptr>
            Value(T initType): type(initType) {}

            template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
            Value(T value): type(Type::FLOAT), doubleValue(static_cast<double>(value)) {}

            template<typename T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
            Value(T value): type(Type::INTEGER), intValue(static_cast<int64_t>(value)) {}

            template<typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
            Value(const T& value): type(Type::STRING), stringValue(value) {}

            template<typename T, typename std::enable_if<std::is_same<T, char>::value>::type* = nullptr>
            Value(const T* value): type(Type::STRING), stringValue(value) {}

            template<typename T, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
            Value(T value): type(Type::BOOLEAN), boolValue(value) {}

            template<typename T, typename std::enable_if<std::is_same<T, std::nullptr_t>::value>::type* = nullptr>
            Value(T): type(Type::OBJECT), nullValue(true) {}

            template<typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
            Value(const T& value): type(Type::ARRAY), arrayValue(value) {}

            template<typename T, typename std::enable_if<std::is_same<T, Object>::value>::type* = nullptr>
            Value(const T& value): type(Type::OBJECT), objectValue(value) {}

            template<typename T, typename std::enable_if<std::is_same<T, Type>::value>::type* = nullptr>
            inline Value& operator=(T newType)
            {
                type = newType;
                return *this;
            }

            template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
            inline Value& operator=(T value)
            {
                type = Type::FLOAT;
                doubleValue = static_cast<double>(value);
                return *this;
            }

            template<typename T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
            inline Value& operator=(T value)
            {
                type = Type::INTEGER;
                intValue = static_cast<int64_t>(value);
                return *this;
            }

            template<typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
            inline Value& operator=(const T& value)
            {
                type = Type::STRING;
                stringValue = value;
                return *this;
            }

            template<typename T, typename std::enable_if<std::is_same<T, char>::value>::type* = nullptr>
            inline Value& operator=(const T* value)
            {
                type = Type::STRING;
                stringValue = value;
                return *this;
            }

            template<typename T, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
            inline Value& operator=(T value)
            {
                type = Type::BOOLEAN;
                boolValue = value;
                return *this;
            }

            template<typename T, typename std::enable_if<std::is_same<T, std::nullptr_t>::value>::type* = nullptr>
            inline Value& operator=(T)
            {
                type = Type::OBJECT;
                nullValue = true;
                objectValue.clear();
                return *this;
            }

            template<typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
            inline Value& operator=(const T& value)
            {
                type = Type::ARRAY;
                arrayValue = value;
                return *this;
            }

            template<typename T, typename std::enable_if<std::is_same<T, Object>::value>::type* = nullptr>
            inline Value& operator=(const T& value)
            {
                type = Type::OBJECT;
                objectValue = value;
                nullValue = false;
                return *this;
            }

            inline Type getType() const { return type; }

            template<typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
            const std::string& as() const
            {
                assert(type == Type::STRING);
                return stringValue;
            }

            template<typename T, typename std::enable_if<std::is_same<T, const char*>::value>::type* = nullptr>
            const char* as() const
            {
                assert(type == Type::STRING);
                return stringValue.c_str();
            }

            template<typename T, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
            T as() const
            {
                assert(type == Type::BOOLEAN || type == Type::INTEGER || type == Type::FLOAT);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::INTEGER) return intValue != 0;
                else return doubleValue != 0.0;
            }

            template<typename T, typename std::enable_if<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
            T as() const
            {
                assert(type == Type::BOOLEAN || type == Type::INTEGER || type == Type::FLOAT);
                if (type == Type::BOOLEAN) return boolValue;
                else if (type == Type::INTEGER) return static_cast<T>(intValue);
                else return static_cast<T>(doubleValue);
            }

            template<typename T, typename std::enable_if<std::is_same<T, Object>::value>::type* = nullptr>
            inline Object& as()
            {
                type = Type::OBJECT;
                return objectValue;
            }

            template<typename T, typename std::enable_if<std::is_same<T, Object>::value>::type* = nullptr>
            inline const Object& as() const
            {
                assert(type == Type::OBJECT);
                return objectValue;
            }

            template<typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
            inline Array& as()
            {
                type = Type::ARRAY;
                return arrayValue;
            }

            template<typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
            inline const Array& as() const
            {
                assert(type == Type::ARRAY);
                return arrayValue;
            }

            inline bool isNull() const
            {
                assert(type == Type::OBJECT);
                return nullValue;
            }

            inline bool hasMember(const std::string& member) const
            {
                assert(type == Type::OBJECT);
                return objectValue.find(member) != objectValue.end();
            }

            inline Value& operator[](const std::string& member)
            {
                type = Type::OBJECT;
                nullValue = false;
                return objectValue[member];
            }

            inline Value operator[](const std::string& member) const
            {
                assert(type == Type::OBJECT);

                auto i = objectValue.find(member);
                if (i != objectValue.end()) return i->second;
                else return Value();
            }

            inline Value& operator[](size_t index)
            {
                type = Type::ARRAY;
                if (index >= arrayValue.size()) arrayValue.resize(index + 1);
                return arrayValue[index];
            }

            inline Value operator[](size_t index) const
            {
                assert(type == Type::ARRAY);

                if (index < arrayValue.size()) return arrayValue[index];
                else return Value();
            }

            inline size_t getSize() const
            {
                assert(type == Type::ARRAY);
                return arrayValue.size();
            }

        protected:
            void parseValue(const std::vector<Token>& tokens,
                            std::vector<Token>::const_iterator& iterator)
            {
                if (iterator == tokens.end())
                    throw std::runtime_error("Unexpected end of data");

                if (iterator->type == Token::Type::LEFT_BRACE)
                    return parseObject(tokens, iterator);
                else if (iterator->type == Token::Type::LEFT_BRACKET)
                    return parseArray(tokens, iterator);
                else if (iterator->type == Token::Type::LITERAL_INTEGER)
                {
                    type = Type::INTEGER;
                    intValue = std::stoi(utf8::fromUtf32(iterator->value));
                    ++iterator;
                }
                else if (iterator->type == Token::Type::LITERAL_FLOAT)
                {
                    type = Type::FLOAT;
                    doubleValue = std::stod(utf8::fromUtf32(iterator->value));
                    ++iterator;
                }
                else if (iterator->type == Token::Type::OPERATOR_MINUS)
                {
                    if (++iterator == tokens.end())
                        throw std::runtime_error("Unexpected end of data");

                    if (iterator->type == Token::Type::LITERAL_INTEGER)
                    {
                        type = Type::INTEGER;
                        intValue = -std::stoi(utf8::fromUtf32(iterator->value));
                        ++iterator;
                    }
                    else if (iterator->type == Token::Type::LITERAL_FLOAT)
                    {
                        type = Type::FLOAT;
                        doubleValue = -std::stod(utf8::fromUtf32(iterator->value));
                        ++iterator;
                    }
                    else
                        throw std::runtime_error("Expected a number");
                }
                else if (iterator->type == Token::Type::LITERAL_STRING)
                {
                    type = Type::STRING;
                    stringValue = utf8::fromUtf32(iterator->value);
                    ++iterator;
                }
                else if (iterator->type == Token::Type::KEYWORD_TRUE ||
                         iterator->type == Token::Type::KEYWORD_FALSE)
                {
                    type = Type::BOOLEAN;
                    boolValue = (iterator->type == Token::Type::KEYWORD_TRUE);
                    ++iterator;
                }
                else if (iterator->type == Token::Type::KEYWORD_NULL)
                {
                    type = Type::OBJECT;
                    nullValue = true;
                    ++iterator;
                }
                else
                    throw std::runtime_error("Expected a value");
            }

            void parseObject(const std::vector<Token>& tokens,
                             std::vector<Token>::const_iterator& iterator)
            {
                if (iterator == tokens.end())
                    throw std::runtime_error("Unexpected end of data");

                if (iterator->type != Token::Type::LEFT_BRACE)
                    throw std::runtime_error("Expected a left brace");

                ++iterator; // skip the left brace

                bool first = true;

                for (;;)
                {
                    if (iterator == tokens.end())
                        throw std::runtime_error("Unexpected end of data");

                    if (iterator->type == Token::Type::RIGHT_BRACE)
                    {
                        ++iterator;// skip the right brace
                        break;
                    }

                    if (first)
                        first = false;
                    else
                    {
                        if (iterator->type != Token::Type::COMMA)
                            throw std::runtime_error("Expected a comma");

                        if (++iterator == tokens.end())
                            throw std::runtime_error("Unexpected end of data");
                    }

                    if (iterator->type != Token::Type::LITERAL_STRING)
                        throw std::runtime_error("Expected a string literal");

                    std::string key = utf8::fromUtf32(iterator->value);

                    if (objectValue.find(key) != objectValue.end())
                        throw std::runtime_error("Duplicate key value " + key);

                    if (++iterator == tokens.end())
                        throw std::runtime_error("Unexpected end of data");

                    if (iterator->type != Token::Type::COLON)
                        throw std::runtime_error("Expected a colon");

                    if (++iterator == tokens.end())
                        throw std::runtime_error("Unexpected end of data");

                    Value value;
                    value.parseValue(tokens, iterator);

                    objectValue[key] = value;
                }

                type = Type::OBJECT;
            }

            void parseArray(const std::vector<Token>& tokens,
                            std::vector<Token>::const_iterator& iterator)
            {
                if (iterator == tokens.end())
                    throw std::runtime_error("Unexpected end of data");

                if (iterator->type != Token::Type::LEFT_BRACKET)
                    throw std::runtime_error("Expected a left bracket");

                ++iterator; // skip the left bracket

                bool first = true;

                for (;;)
                {
                    if (iterator == tokens.end())
                        throw std::runtime_error("Unexpected end of data");

                    if (iterator->type == Token::Type::RIGHT_BRACKET)
                    {
                        ++iterator;// skip the right bracket
                        break;
                    }

                    if (first)
                        first = false;
                    else
                    {
                        if (iterator->type != Token::Type::COMMA)
                            throw std::runtime_error("Expected a comma");

                        if (++iterator == tokens.end())
                            throw std::runtime_error("Unexpected end of data");
                    }

                    Value value;
                    value.parseValue(tokens, iterator);

                    arrayValue.push_back(value);
                }

                type = Type::ARRAY;
            }

            static void encodeString(std::vector<uint8_t>& data,
                                     const std::vector<uint32_t>& str)
            {
                for (uint32_t c : str)
                {
                    if (c == '"') data.insert(data.end(), {'\\', '"'});
                    else if (c == '\\') data.insert(data.end(), {'\\', '\\'});
                    else if (c == '/') data.insert(data.end(), {'\\', '/'});
                    else if (c == '\b') data.insert(data.end(), {'\\', 'b'});
                    else if (c == '\f') data.insert(data.end(), {'\\', 'f'});
                    else if (c == '\n') data.insert(data.end(), {'\\', 'n'});
                    else if (c == '\r') data.insert(data.end(), {'\\', 'r'});
                    else if (c == '\t') data.insert(data.end(), {'\\', 't'});
                    else if (c <= 0x1F)
                    {
                        data.insert(data.end(), {'\\', 'u'});

                        static constexpr const char* digits = "0123456789ABCDEF";
                        for (uint32_t p = 0; p < 4; ++p)
                            data.push_back(static_cast<uint8_t>(digits[(c >> (12 - p * 4)) & 0x0F]));
                    }
                    else
                    {
                        std::string encoded = utf8::fromUtf32(c);
                        data.insert(data.end(), encoded.begin(), encoded.end());
                    }
                }
            }

            void encodeValue(std::vector<uint8_t>& data) const
            {
                switch (type)
                {
                    case Type::INTEGER:
                    {
                        std::string value = std::to_string(intValue);
                        data.insert(data.end(), value.begin(), value.end());
                        break;
                    }
                    case Type::FLOAT:
                    {
                        std::string value = std::to_string(doubleValue);
                        data.insert(data.end(), value.begin(), value.end());
                        break;
                    }
                    case Type::STRING:
                        data.push_back('"');
                        encodeString(data, utf8::toUtf32(stringValue));
                        data.push_back('"');
                        break;
                    case Type::OBJECT:
                    {
                        if (nullValue)
                            data.insert(data.end(), {'n', 'u', 'l', 'l'});
                        else
                        {
                            data.push_back('{');

                            bool first = true;

                            for (const auto& value : objectValue)
                            {
                                if (first) first = false;
                                else data.push_back(',');

                                data.push_back('"');
                                encodeString(data, utf8::toUtf32(value.first));
                                data.insert(data.end(), {'"', ':'});
                                value.second.encodeValue(data);
                            }

                            data.push_back('}');
                        }
                        break;
                    }
                    case Type::ARRAY:
                    {
                        data.push_back('[');

                        bool first = true;

                        for (const auto& value : arrayValue)
                        {
                            if (first) first = false;
                            else data.push_back(',');

                            value.encodeValue(data);
                        }

                        data.push_back(']');
                        break;
                    }
                    case Type::BOOLEAN:
                        if (boolValue) data.insert(data.end(), {'t', 'r', 'u', 'e'});
                        else data.insert(data.end(), {'f', 'a', 'l', 's', 'e'});
                        break;
                    default:
                        throw std::runtime_error("Unknown value type");
                }
            }

        private:
            Type type = Type::NONE;
            union
            {
                bool boolValue = false;
                bool nullValue;
                int64_t intValue;
                double doubleValue;
            };
            Object objectValue;
            Array arrayValue;
            std::string stringValue;
        };

        class Data final: public Value
        {
        public:
            Data(): Value(Value::Type::OBJECT)
            {
            }

            Data(const std::vector<uint8_t>& data)
            {
                std::vector<uint32_t> str;

                // BOM
                if (data.size() >= 3 &&
                    data[0] == 0xEF &&
                    data[1] == 0xBB &&
                    data[2] == 0xBF)
                {
                    bom = true;
                    str = utf8::toUtf32(std::vector<uint8_t>(data.begin() + 3, data.end()));
                }
                else
                {
                    bom = false;
                    str = utf8::toUtf32(data);
                }

                std::vector<Token> tokens = tokenize(str);

                auto iterator = tokens.cbegin();

                parseValue(tokens, iterator);
            }

            std::vector<uint8_t> encode() const
            {
                std::vector<uint8_t> result;

                if (bom) result = {0xEF, 0xBB, 0xBF};

                encodeValue(result);

                return result;
            }

            inline bool hasBOM() const { return bom; }
            inline void setBOM(bool newBOM) { bom = newBOM; }

        private:
            static std::vector<Token> tokenize(const std::vector<uint32_t>& str)
            {
                std::vector<Token> tokens;

                static const std::map<std::vector<uint32_t>, Token::Type> keywordMap = {
                    {{'t', 'r', 'u', 'e'}, Token::Type::KEYWORD_TRUE},
                    {{'f', 'a', 'l', 's', 'e'}, Token::Type::KEYWORD_FALSE},
                    {{'n', 'u', 'l', 'l'}, Token::Type::KEYWORD_NULL}
                };

                // tokenize
                for (auto iterator = str.cbegin(); iterator != str.cend();)
                {
                    Token token;

                    if (*iterator == '{' || *iterator == '}' ||
                        *iterator == '[' || *iterator == ']' ||
                        *iterator == ',' || *iterator == ':') // punctuation
                    {
                        if (*iterator == '{') token.type = Token::Type::LEFT_BRACE;
                        if (*iterator == '}') token.type = Token::Type::RIGHT_BRACE;
                        if (*iterator == '[') token.type = Token::Type::LEFT_BRACKET;
                        if (*iterator == ']') token.type = Token::Type::RIGHT_BRACKET;
                        if (*iterator == ',') token.type = Token::Type::COMMA;
                        if (*iterator == ':') token.type = Token::Type::COLON;

                        ++iterator;
                    }
                    else if (*iterator >= '0' && *iterator <= '9')
                    {
                        token.type = Token::Type::LITERAL_INTEGER;

                        while (iterator != str.end() &&
                               (*iterator >= '0' && *iterator <= '9'))
                        {
                            token.value.push_back(*iterator);
                            ++iterator;
                        }

                        if (iterator != str.end() && *iterator == '.')
                        {
                            token.type = Token::Type::LITERAL_FLOAT;

                            token.value.push_back(*iterator);
                            ++iterator;

                            while (iterator != str.end() &&
                                   (*iterator >= '0' && *iterator <= '9'))
                            {
                                token.value.push_back(*iterator);
                                ++iterator;
                            }
                        }

                        // parse exponent
                        if (iterator != str.end() &&
                            (*iterator == 'e' || *iterator == 'E'))
                        {
                            token.value.push_back(*iterator);

                            if (++iterator == str.end())
                                throw std::runtime_error("Invalid exponent");

                            if (*iterator == '+' || *iterator == '-')
                                token.value.push_back(*iterator);

                            if (++iterator == str.end() || *iterator < '0' || *iterator > '9')
                                throw std::runtime_error("Invalid exponent");

                            while (iterator != str.end() &&
                                   (*iterator >= '0' && *iterator <= '9'))
                            {
                                token.value.push_back(*iterator);
                                ++iterator;
                            }
                        }
                    }
                    else if (*iterator == '"') // string literal
                    {
                        token.type = Token::Type::LITERAL_STRING;

                        for (;;)
                        {
                            if (++iterator == str.end())
                                throw std::runtime_error("Unterminated string literal");

                            if (*iterator == '"')
                            {
                                ++iterator;
                                break;
                            }
                            else if (*iterator == '\\')
                            {
                                if (++iterator == str.end())
                                    throw std::runtime_error("Unterminated string literal");

                                if (*iterator == '"') token.value.push_back('"');
                                else if (*iterator == '\\') token.value.push_back('\\');
                                else if (*iterator == '/') token.value.push_back('/');
                                else if (*iterator == 'b') token.value.push_back('\b');
                                else if (*iterator == 'f') token.value.push_back('\f');
                                else if (*iterator == 'n') token.value.push_back('\n');
                                else if (*iterator == 'r') token.value.push_back('\r');
                                else if (*iterator == 't') token.value.push_back('\t');
                                else if (*iterator == 'u')
                                {
                                    if (std::distance<std::vector<uint32_t>::const_iterator>(++iterator, str.end()) < 4)
                                        throw std::runtime_error("Unexpected end of data");

                                    uint32_t c = 0;

                                    for (uint32_t i = 0; i < 4; ++i, ++iterator)
                                    {
                                        uint8_t code = 0;

                                        if (*iterator >= '0' && *iterator <= '9') code = static_cast<uint8_t>(*iterator) - '0';
                                        else if (*iterator >= 'a' && *iterator <='f') code = static_cast<uint8_t>(*iterator) - 'a' + 10;
                                        else if (*iterator >= 'A' && *iterator <='F') code = static_cast<uint8_t>(*iterator) - 'A' + 10;
                                        else
                                            throw std::runtime_error("Invalid character code");

                                        c = (c << 4) | code;
                                    }

                                    token.value.push_back(c);
                                }
                                else
                                    throw std::runtime_error("Unrecognized escape character");
                            }
                            else if (*iterator <= 0x1F) // control char
                                throw std::runtime_error("Unterminated string literal");
                            else
                                token.value.push_back(*iterator);
                        }
                    }
                    else if ((*iterator >= 'a' && *iterator <= 'z') ||
                             (*iterator >= 'A' && *iterator <= 'Z') ||
                             *iterator == '_')
                    {
                        while (iterator != str.end() &&
                               ((*iterator >= 'a' && *iterator <= 'z') ||
                                (*iterator >= 'A' && *iterator <= 'Z') ||
                                *iterator == '_' ||
                                (*iterator >= '0' && *iterator <= '9')))
                        {
                            token.value.push_back(*iterator);
                            ++iterator;
                        }

                        std::map<std::vector<uint32_t>, Token::Type>::const_iterator keywordIterator;

                        if ((keywordIterator = keywordMap.find(token.value)) != keywordMap.end())
                            token.type = keywordIterator->second;
                        else
                            throw std::runtime_error("Unknown keyword " + utf8::fromUtf32(token.value));
                    }
                    else if (*iterator == '-')
                    {
                        token.type = Token::Type::OPERATOR_MINUS;
                        ++iterator;
                    }
                    else if (*iterator == ' ' ||
                             *iterator == '\t' ||
                             *iterator == '\r' ||
                             *iterator == '\n') // whitespace or newline
                    {
                        ++iterator;
                        continue;
                    }
                    else
                        throw std::runtime_error("Unknown character");

                    tokens.push_back(token);
                }

                return tokens;
            }

            bool bom = false;
        };
    } // namespace json
} // namespace ouzel

#endif // OUZEL_UTILS_JSON_HPP
