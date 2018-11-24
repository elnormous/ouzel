// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "JSON.hpp"
#include "Utils.hpp"

namespace ouzel
{
    namespace json
    {
        void Value::parseValue(const std::vector<Token>& tokens,
                               std::vector<Token>::const_iterator& iterator)
        {
            if (iterator == tokens.end())
                throw std::runtime_error("Unexpected end of data");

            if (iterator->type == Token::Type::LEFT_BRACE)
                return parseObject(tokens, iterator);
            else if (iterator->type == Token::Type::LEFT_BRACKET)
                return parseArray(tokens, iterator);
            else if (iterator->type == Token::Type::LITERAL_NUMBER)
            {
                type = Type::NUMBER;
                doubleValue = std::stod(utf32ToUtf8(iterator->value));
                ++iterator;
            }
            else if (iterator->type == Token::Type::OPERATOR_MINUS)
            {
                if (++iterator == tokens.end())
                    throw std::runtime_error("Unexpected end of data");

                if (iterator->type != Token::Type::LITERAL_NUMBER)
                    throw std::runtime_error("Expected a number");

                type = Type::NUMBER;
                doubleValue = -std::stod(utf32ToUtf8(iterator->value));
                ++iterator;
            }
            else if (iterator->type == Token::Type::LITERAL_STRING)
            {
                type = Type::STRING;
                stringValue = utf32ToUtf8(iterator->value);
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

        void Value::parseObject(const std::vector<Token>& tokens,
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

                std::string key = utf32ToUtf8(iterator->value);

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

        void Value::parseArray(const std::vector<Token>& tokens,
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
                    std::string hexValue = hexToString(c, 4);
                    data.insert(data.end(), hexValue.begin(), hexValue.end());
                }
                else
                {
                    std::string encoded = utf32ToUtf8(c);
                    data.insert(data.end(), encoded.begin(), encoded.end());
                }
            }
        }

        void Value::encodeValue(std::vector<uint8_t>& data) const
        {
            switch (type)
            {
                case Type::NUMBER:
                {
                    std::string value = std::to_string(doubleValue);
                    data.insert(data.end(), value.begin(), value.end());
                    break;
                }
                case Type::STRING:
                    data.push_back('"');
                    encodeString(data, utf8ToUtf32(stringValue));
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
                            encodeString(data, utf8ToUtf32(value.first));
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

        Data::Data():
            Value(Value::Type::OBJECT)
        {
        }

        static inline bool isWhitespace(uint32_t c)
        {
            return c == ' ' || c == '\t' || c == '\r' || c == '\n';
        }

        static inline bool isControlChar(uint32_t c)
        {
            return c <= 0x1F;
        }

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
                    token.value.push_back(*iterator);

                    ++iterator;
                }
                else if ((*iterator >= '0' && *iterator <= '9') ||  // number
                         (*iterator == '.' && (iterator + 1) != str.end() &&
                          *(iterator + 1) >= '0' && *(iterator + 1) <= '9')) // starts with a dot
                {
                    token.type = Token::Type::LITERAL_NUMBER;

                    while (iterator != str.end() &&
                           (*iterator >= '0' && *iterator <= '9'))
                    {
                        token.value.push_back(*iterator);
                        ++iterator;
                    }

                    if (iterator != str.end() && *iterator == '.')
                    {
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

                        if (++iterator == str.end() || (*iterator != '+' && *iterator != '-'))
                            throw std::runtime_error("Invalid exponent");

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
                        else if (isControlChar(*iterator))
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
                        throw std::runtime_error("Unknown keyword " + utf32ToUtf8(token.value));
                }
                else if (*iterator == '-')
                {
                    token.type = Token::Type::OPERATOR_MINUS;
                    token.value.push_back(*iterator);
                    ++iterator;
                }
                else if (isWhitespace(*iterator)) // whitespace
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

        Data::Data(const std::vector<uint8_t>& data)
        {
            std::vector<uint32_t> str;

            // BOM
            if (data.size() >= 3 &&
                data[0] == 0xEF &&
                data[1] == 0xBB &&
                data[2] == 0xBF)
            {
                bom = true;
                str = utf8ToUtf32(std::vector<uint8_t>(data.begin() + 3, data.end()));
            }
            else
            {
                bom = false;
                str = utf8ToUtf32(data);
            }

            std::vector<Token> tokens = tokenize(str);

            auto iterator = tokens.cbegin();

            parseValue(tokens, iterator);
        }

        std::vector<uint8_t> Data::encode() const
        {
            std::vector<uint8_t> result;

            if (bom) result = {0xEF, 0xBB, 0xBF};

            encodeValue(result);

            return result;
        }
    } // namespace json
} // namespace ouzel
