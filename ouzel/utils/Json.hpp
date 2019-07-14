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
        static const std::vector<uint8_t> UTF8_BOM = {0xEF, 0xBB, 0xBF};

        inline void encodeString(std::vector<uint8_t>& data,
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

        struct Token final
        {
            enum class Type
            {
                LiteralInteger, // integer
                LiteralFloat, // float
                LiteralString, // string
                KeywordTrue, // true
                KeywordFalse, // false
                KeywordNull, // null
                LeftBrace, // {
                RightBrace, // }
                LeftBracket, // [
                RightBracket, // ]
                Comma, // ,
                Colon // :
            };

            Type type;
            std::vector<uint32_t> value;
        };
        
        inline std::vector<Token> tokenize(const std::vector<uint32_t>& str)
        {
            std::vector<Token> tokens;

            static const std::map<std::vector<uint32_t>, Token::Type> keywordMap{
                {{'t', 'r', 'u', 'e'}, Token::Type::KeywordTrue},
                {{'f', 'a', 'l', 's', 'e'}, Token::Type::KeywordFalse},
                {{'n', 'u', 'l', 'l'}, Token::Type::KeywordNull}
            };

            // tokenize
            for (auto iterator = str.cbegin(); iterator != str.cend();)
            {
                Token token;

                if (*iterator == '{' || *iterator == '}' ||
                    *iterator == '[' || *iterator == ']' ||
                    *iterator == ',' || *iterator == ':') // punctuation
                {
                    if (*iterator == '{') token.type = Token::Type::LeftBrace;
                    if (*iterator == '}') token.type = Token::Type::RightBrace;
                    if (*iterator == '[') token.type = Token::Type::LeftBracket;
                    if (*iterator == ']') token.type = Token::Type::RightBracket;
                    if (*iterator == ',') token.type = Token::Type::Comma;
                    if (*iterator == ':') token.type = Token::Type::Colon;

                    ++iterator;
                }
                else if (*iterator == '-' ||
                         (*iterator >= '0' && *iterator <= '9'))
                {
                    token.type = Token::Type::LiteralInteger;

                    if (*iterator == '-')
                    {
                        token.value.push_back(*iterator);
                        if (++iterator == str.cend() ||
                            *iterator < '0' || *iterator > '9')
                            throw std::runtime_error("Invalid number");
                    }

                    while (iterator != str.cend() &&
                           (*iterator >= '0' && *iterator <= '9'))
                    {
                        token.value.push_back(*iterator);
                        ++iterator;
                    }

                    if (iterator != str.cend() && *iterator == '.')
                    {
                        token.type = Token::Type::LiteralFloat;

                        token.value.push_back(*iterator);
                        ++iterator;

                        while (iterator != str.cend() &&
                               (*iterator >= '0' && *iterator <= '9'))
                        {
                            token.value.push_back(*iterator);
                            ++iterator;
                        }
                    }

                    // parse exponent
                    if (iterator != str.cend() &&
                        (*iterator == 'e' || *iterator == 'E'))
                    {
                        token.value.push_back(*iterator);

                        if (++iterator == str.cend())
                            throw std::runtime_error("Invalid exponent");

                        if (*iterator == '+' || *iterator == '-')
                            token.value.push_back(*iterator);

                        if (++iterator == str.cend() || *iterator < '0' || *iterator > '9')
                            throw std::runtime_error("Invalid exponent");

                        while (iterator != str.cend() &&
                               (*iterator >= '0' && *iterator <= '9'))
                        {
                            token.value.push_back(*iterator);
                            ++iterator;
                        }
                    }
                }
                else if (*iterator == '"') // string literal
                {
                    token.type = Token::Type::LiteralString;

                    for (;;)
                    {
                        if (++iterator == str.cend())
                            throw std::runtime_error("Unterminated string literal");

                        if (*iterator == '"')
                        {
                            ++iterator;
                            break;
                        }
                        else if (*iterator == '\\')
                        {
                            if (++iterator == str.cend())
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
                                if (std::distance(++iterator, str.cend()) < 4)
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
                    while (iterator != str.cend() &&
                           ((*iterator >= 'a' && *iterator <= 'z') ||
                            (*iterator >= 'A' && *iterator <= 'Z') ||
                            *iterator == '_' ||
                            (*iterator >= '0' && *iterator <= '9')))
                    {
                        token.value.push_back(*iterator);
                        ++iterator;
                    }

                    auto keywordIterator = keywordMap.find(token.value);

                    if (keywordIterator != keywordMap.end())
                        token.type = keywordIterator->second;
                    else
                        throw std::runtime_error("Unknown keyword " + utf8::fromUtf32(token.value));
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

        class Value
        {
        public:
            using Array = std::vector<Value>;
            using Object = std::map<std::string, Value>;

            enum class Type
            {
                Null,
                Integer,
                Float,
                String,
                Object,
                Array,
                Boolean
            };

            Value() = default;

            template <typename T, typename std::enable_if<std::is_same<T, Type>::value>::type* = nullptr>
            Value(T initType): type(initType) {}

            template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
            Value(T value): type(Type::Float), doubleValue(static_cast<double>(value)) {}

            template <typename T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
            Value(T value): type(Type::Integer), intValue(static_cast<int64_t>(value)) {}

            template <typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
            Value(const T& value): type(Type::String), stringValue(value) {}

            template <typename T, typename std::enable_if<std::is_same<T, char>::value>::type* = nullptr>
            Value(const T* value): type(Type::String), stringValue(value) {}

            template <typename T, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
            Value(T value): type(Type::Boolean), boolValue(value) {}

            template <typename T, typename std::enable_if<std::is_same<T, std::nullptr_t>::value>::type* = nullptr>
            Value(T): type(Type::Null) {}

            template <typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
            Value(const T& value): type(Type::Array), arrayValue(value) {}

            template <typename T, typename std::enable_if<std::is_same<T, Object>::value>::type* = nullptr>
            Value(const T& value): type(Type::Object), objectValue(value) {}

            template <typename T, typename std::enable_if<std::is_same<T, Type>::value>::type* = nullptr>
            inline Value& operator=(T newType)
            {
                type = newType;
                return *this;
            }

            template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
            inline Value& operator=(T value)
            {
                type = Type::Float;
                doubleValue = static_cast<double>(value);
                return *this;
            }

            template <typename T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
            inline Value& operator=(T value)
            {
                type = Type::Integer;
                intValue = static_cast<int64_t>(value);
                return *this;
            }

            template <typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
            inline Value& operator=(const T& value)
            {
                type = Type::String;
                stringValue = value;
                return *this;
            }

            template <typename T, typename std::enable_if<std::is_same<T, char>::value>::type* = nullptr>
            inline Value& operator=(const T* value)
            {
                type = Type::String;
                stringValue = value;
                return *this;
            }

            template <typename T, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
            inline Value& operator=(T value)
            {
                type = Type::Boolean;
                boolValue = value;
                return *this;
            }

            template <typename T, typename std::enable_if<std::is_same<T, std::nullptr_t>::value>::type* = nullptr>
            inline Value& operator=(T)
            {
                type = Type::Null;
                objectValue.clear();
                return *this;
            }

            template <typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
            inline Value& operator=(const T& value)
            {
                type = Type::Array;
                arrayValue = value;
                return *this;
            }

            template <typename T, typename std::enable_if<std::is_same<T, Object>::value>::type* = nullptr>
            inline Value& operator=(const T& value)
            {
                type = Type::Object;
                objectValue = value;
                return *this;
            }

            inline auto getType() const noexcept { return type; }

            template <typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
            const std::string& as() const
            {
                assert(type == Type::String);
                return stringValue;
            }

            template <typename T, typename std::enable_if<std::is_same<T, const char*>::value>::type* = nullptr>
            const char* as() const
            {
                assert(type == Type::String);
                return stringValue.c_str();
            }

            template <typename T, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
            T as() const
            {
                assert(type == Type::Boolean || type == Type::Integer || type == Type::Float);
                if (type == Type::Boolean) return boolValue;
                else if (type == Type::Integer) return intValue != 0;
                else return doubleValue != 0.0;
            }

            template <typename T, typename std::enable_if<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
            T as() const
            {
                assert(type == Type::Boolean || type == Type::Integer || type == Type::Float);
                if (type == Type::Boolean) return boolValue;
                else if (type == Type::Integer) return static_cast<T>(intValue);
                else return static_cast<T>(doubleValue);
            }

            template <typename T, typename std::enable_if<std::is_same<T, Object>::value>::type* = nullptr>
            inline Object& as()
            {
                type = Type::Object;
                return objectValue;
            }

            template <typename T, typename std::enable_if<std::is_same<T, Object>::value>::type* = nullptr>
            inline const Object& as() const
            {
                assert(type == Type::Object);
                return objectValue;
            }

            template <typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
            inline Array& as()
            {
                type = Type::Array;
                return arrayValue;
            }

            template <typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
            inline const Array& as() const
            {
                assert(type == Type::Array);
                return arrayValue;
            }

            Array::iterator begin()
            {
                assert(type == Type::Array);
                return arrayValue.begin();
            }

            Array::iterator end()
            {
                assert(type == Type::Array);
                return arrayValue.end();
            }

            Array::const_iterator begin() const
            {
                assert(type == Type::Array);
                return arrayValue.begin();
            }

            Array::const_iterator end() const
            {
                assert(type == Type::Array);
                return arrayValue.end();
            }

            inline auto isNull() const noexcept
            {
                return type == Type::Null;
            }

            inline auto hasMember(const std::string& member) const
            {
                assert(type == Type::Object);
                return objectValue.find(member) != objectValue.end();
            }

            inline Value& operator[](const std::string& member)
            {
                type = Type::Object;
                return objectValue[member];
            }

            inline const Value& operator[](const std::string& member) const
            {
                assert(type == Type::Object);

                auto i = objectValue.find(member);
                if (i != objectValue.end())
                    return i->second;
                else
                    throw std::runtime_error("Member does not exist");
            }

            inline Value& operator[](size_t index)
            {
                type = Type::Array;
                if (index >= arrayValue.size()) arrayValue.resize(index + 1);
                return arrayValue[index];
            }

            inline const Value& operator[](size_t index) const
            {
                assert(type == Type::Array);

                if (index < arrayValue.size())
                    return arrayValue[index];
                else
                    throw std::runtime_error("Index out of range");
            }

            inline auto getSize() const
            {
                assert(type == Type::Array);
                return arrayValue.size();
            }

        protected:
            template <class TokenIterator>
            void parseValue(TokenIterator& iterator,
                            TokenIterator end)
            {
                if (iterator == end)
                    throw std::runtime_error("Unexpected end of data");

                if (iterator->type == Token::Type::LeftBrace)
                    return parseObject(iterator, end);
                else if (iterator->type == Token::Type::LeftBracket)
                    return parseArray(iterator, end);
                else if (iterator->type == Token::Type::LiteralInteger)
                {
                    type = Type::Integer;
                    intValue = std::stoll(utf8::fromUtf32(iterator->value));
                    ++iterator;
                }
                else if (iterator->type == Token::Type::LiteralFloat)
                {
                    type = Type::Float;
                    doubleValue = std::stod(utf8::fromUtf32(iterator->value));
                    ++iterator;
                }
                else if (iterator->type == Token::Type::LiteralString)
                {
                    type = Type::String;
                    stringValue = utf8::fromUtf32(iterator->value);
                    ++iterator;
                }
                else if (iterator->type == Token::Type::KeywordTrue ||
                         iterator->type == Token::Type::KeywordFalse)
                {
                    type = Type::Boolean;
                    boolValue = (iterator->type == Token::Type::KeywordTrue);
                    ++iterator;
                }
                else if (iterator->type == Token::Type::KeywordNull)
                {
                    type = Type::Null;
                    ++iterator;
                }
                else
                    throw std::runtime_error("Expected a value");
            }

            template <class TokenIterator>
            void parseObject(TokenIterator& iterator,
                             TokenIterator end)
            {
                if (iterator == end)
                    throw std::runtime_error("Unexpected end of data");

                if (iterator->type != Token::Type::LeftBrace)
                    throw std::runtime_error("Expected a left brace");

                ++iterator; // skip the left brace

                bool first = true;

                for (;;)
                {
                    if (iterator == end)
                        throw std::runtime_error("Unexpected end of data");

                    if (iterator->type == Token::Type::RightBrace)
                    {
                        ++iterator;// skip the right brace
                        break;
                    }

                    if (first)
                        first = false;
                    else
                    {
                        if (iterator->type != Token::Type::Comma)
                            throw std::runtime_error("Expected a comma");

                        if (++iterator == end)
                            throw std::runtime_error("Unexpected end of data");
                    }

                    if (iterator->type != Token::Type::LiteralString)
                        throw std::runtime_error("Expected a string literal");

                    std::string key = utf8::fromUtf32(iterator->value);

                    if (objectValue.find(key) != objectValue.end())
                        throw std::runtime_error("Duplicate key value " + key);

                    if (++iterator == end)
                        throw std::runtime_error("Unexpected end of data");

                    if (iterator->type != Token::Type::Colon)
                        throw std::runtime_error("Expected a colon");

                    if (++iterator == end)
                        throw std::runtime_error("Unexpected end of data");

                    Value value;
                    value.parseValue(iterator, end);

                    objectValue[key] = value;
                }

                type = Type::Object;
            }

            template <class TokenIterator>
            void parseArray(TokenIterator& iterator,
                            TokenIterator end)
            {
                if (iterator == end)
                    throw std::runtime_error("Unexpected end of data");

                if (iterator->type != Token::Type::LeftBracket)
                    throw std::runtime_error("Expected a left bracket");

                ++iterator; // skip the left bracket

                bool first = true;

                for (;;)
                {
                    if (iterator == end)
                        throw std::runtime_error("Unexpected end of data");

                    if (iterator->type == Token::Type::RightBracket)
                    {
                        ++iterator;// skip the right bracket
                        break;
                    }

                    if (first)
                        first = false;
                    else
                    {
                        if (iterator->type != Token::Type::Comma)
                            throw std::runtime_error("Expected a comma");

                        if (++iterator == end)
                            throw std::runtime_error("Unexpected end of data");
                    }

                    Value value;
                    value.parseValue(iterator, end);

                    arrayValue.push_back(value);
                }

                type = Type::Array;
            }

            void encodeValue(std::vector<uint8_t>& data) const
            {
                switch (type)
                {
                    case Type::Null:
                    {
                        data.insert(data.end(), {'n', 'u', 'l', 'l'});
                        break;
                    }
                    case Type::Integer:
                    {
                        std::string value = std::to_string(intValue);
                        data.insert(data.end(), value.begin(), value.end());
                        break;
                    }
                    case Type::Float:
                    {
                        std::string value = std::to_string(doubleValue);
                        data.insert(data.end(), value.begin(), value.end());
                        break;
                    }
                    case Type::String:
                        data.push_back('"');
                        encodeString(data, utf8::toUtf32(stringValue));
                        data.push_back('"');
                        break;
                    case Type::Object:
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
                        break;
                    }
                    case Type::Array:
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
                    case Type::Boolean:
                        if (boolValue) data.insert(data.end(), {'t', 'r', 'u', 'e'});
                        else data.insert(data.end(), {'f', 'a', 'l', 's', 'e'});
                        break;
                    default:
                        throw std::runtime_error("Unknown value type");
                }
            }

        private:
            Type type = Type::Object;
            union
            {
                bool boolValue = false;
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
            Data(): Value(Value::Type::Object)
            {
            }

            Data(const std::vector<uint8_t>& data)
            {
                std::vector<uint32_t> str;

                // BOM
                if (data.size() >= 3 &&
                    data[0] == UTF8_BOM[0] &&
                    data[1] == UTF8_BOM[1] &&
                    data[2] == UTF8_BOM[2])
                {
                    bom = true;
                    str = utf8::toUtf32(data.begin() + 3, data.end());
                }
                else
                {
                    bom = false;
                    str = utf8::toUtf32(data);
                }

                std::vector<Token> tokens = tokenize(str);

                auto iterator = tokens.cbegin();

                parseValue(iterator, tokens.cend());
            }

            std::vector<uint8_t> encode() const
            {
                std::vector<uint8_t> result;

                if (bom) result = UTF8_BOM;

                encodeValue(result);

                return result;
            }

            inline bool hasBOM() const { return bom; }
            inline void setBOM(bool newBOM) { bom = newBOM; }

        private:
            bool bom = false;
        };
    } // namespace json
} // namespace ouzel

#endif // OUZEL_UTILS_JSON_HPP
