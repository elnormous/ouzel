// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "JSON.hpp"
#include "core/Engine.hpp"
#include "Log.hpp"
#include "Utils.hpp"

namespace ouzel
{
    namespace json
    {
        bool Value::parseValue(const std::vector<Token>& tokens,
                               std::vector<Token>::const_iterator&iterator)
        {
            if (iterator == tokens.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of data";
                return false;
            }

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
                {
                    Log(Log::Level::ERR) << "Unexpected end of data";
                    return false;
                }

                if (iterator->type != Token::Type::LITERAL_NUMBER)
                {
                    Log(Log::Level::ERR) << "Expected a number";
                    return false;
                }

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
            {
                Log(Log::Level::ERR) << "Expected a value";
                return false;
            }

            return true;
        }

        bool Value::parseObject(const std::vector<Token>& tokens,
                                std::vector<Token>::const_iterator&iterator)
        {
            if (iterator == tokens.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of data";
                return false;
            }

            if (iterator->type != Token::Type::LEFT_BRACE)
            {
                Log(Log::Level::ERR) << "Expected a left brace";
                return false;
            }

            ++iterator; // skip the left brace

            bool first = true;

            for (;;)
            {
                if (iterator == tokens.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of data";
                    return false;
                }

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
                    {
                        Log(Log::Level::ERR) << "Expected a comma";
                        return false;
                    }

                    if (++iterator == tokens.end())
                    {
                        Log(Log::Level::ERR) << "Unexpected end of data";
                        return false;
                    }
                }

                if (iterator->type != Token::Type::LITERAL_STRING)
                {
                    Log(Log::Level::ERR) << "Expected a string literal";
                    return false;
                }

                std::string key = utf32ToUtf8(iterator->value);

                if (objectValue.find(key) != objectValue.end())
                {
                    Log(Log::Level::ERR) << "Duplicate key value";
                    return false;
                }

                if (++iterator == tokens.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of data";
                    return false;
                }

                if (iterator->type != Token::Type::COLON)
                {
                    Log(Log::Level::ERR) << "Expected a colon";
                    return false;
                }

                if (++iterator == tokens.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of data";
                    return false;
                }

                Value value;
                if (!value.parseValue(tokens, iterator))
                    return false;

                objectValue[key] = value;
            }

            type = Type::OBJECT;

            return true;
        }

        bool Value::parseArray(const std::vector<Token>& tokens,
                               std::vector<Token>::const_iterator&iterator)
        {
            if (iterator == tokens.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of data";
                return false;
            }

            if (iterator->type != Token::Type::LEFT_BRACKET)
            {
                Log(Log::Level::ERR) << "Expected a left bracket";
                return false;
            }

            ++iterator; // skip the left bracket

            bool first = true;

            for (;;)
            {
                if (iterator == tokens.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of data";
                    return false;
                }

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
                    {
                        Log(Log::Level::ERR) << "Expected a comma";
                        return false;
                    }

                    if (++iterator == tokens.end())
                    {
                        Log(Log::Level::ERR) << "Unexpected end of data";
                        return false;
                    }
                }

                Value value;
                if (!value.parseValue(tokens, iterator))
                    return false;

                arrayValue.push_back(value);
            }

            type = Type::ARRAY;

            return true;
        }

        static bool encodeString(std::vector<uint8_t>& data,
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

            return true;
        }

        bool Value::encodeValue(std::vector<uint8_t>& data) const
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
                    if (!encodeString(data, utf8ToUtf32(stringValue))) return false;
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
                            if (!encodeString(data, utf8ToUtf32(value.first))) return false;
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
                    return false;
            }

            return true;
        }

        Data::Data():
            Value(Value::Type::OBJECT)
        {
        }

        Data::Data(const std::string& filename)
        {
            init(filename);
        }

        Data::Data(const std::vector<uint8_t>& data)
        {
            init(data);
        }

        bool Data::init(const std::string& filename)
        {
            std::vector<uint8_t> data;
            engine->getFileSystem()->readFile(filename, data);

            return init(data);
        }

        static inline bool isWhitespace(uint32_t c)
        {
            return c == ' ' || c == '\t' || c == '\r' || c == '\n';
        }

        static inline bool isControlChar(uint32_t c)
        {
            return c <= 0x1F;
        }

        static bool tokenize(const std::vector<uint32_t>& str, std::vector<Token>& tokens)
        {
            tokens.clear();

            static const std::map<std::vector<uint32_t>, Token::Type> keywordMap = {
                {{'t', 'r', 'u', 'e'}, Token::Type::KEYWORD_TRUE},
                {{'f', 'a', 'l', 's', 'e'}, Token::Type::KEYWORD_FALSE},
                {{'n', 'u', 'l', 'l'}, Token::Type::KEYWORD_NULL}
            };

            // tokenize
            for (std::vector<uint32_t>::const_iterator iterator = str.begin(); iterator != str.end();)
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

                        if (++iterator == str.end() || *iterator != '+' || *iterator != '-')
                        {
                            Log(Log::Level::ERR) << "Invalid exponent";
                            return false;
                        }

                        token.value.push_back(*iterator);

                        if (++iterator == str.end() || *iterator < '0' || *iterator > '9')
                        {
                            Log(Log::Level::ERR) << "Invalid exponent";
                            return false;
                        }

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
                        {
                            Log(Log::Level::ERR) << "Unterminated string literal";
                            return false;
                        }

                        if (*iterator == '"')
                        {
                            ++iterator;
                            break;
                        }
                        else if (*iterator == '\\')
                        {
                            if (++iterator == str.end())
                            {
                                Log(Log::Level::ERR) << "Unterminated string literal";
                                return false;
                            }

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
                                {
                                    Log(Log::Level::ERR) << "Unexpected end of data";
                                    return false;
                                }

                                uint32_t c = 0;

                                for (uint32_t i = 0; i < 4; ++i, ++iterator)
                                {
                                    uint8_t code = 0;

                                    if (*iterator >= '0' && *iterator <= '9') code = static_cast<uint8_t>(*iterator) - '0';
                                    else if (*iterator >= 'a' && *iterator <='f') code = static_cast<uint8_t>(*iterator) - 'a' + 10;
                                    else if (*iterator >= 'A' && *iterator <='F') code = static_cast<uint8_t>(*iterator) - 'A' + 10;
                                    else
                                    {
                                        Log(Log::Level::ERR) << "Invalid character code";
                                        return false;
                                    }
                                    c = (c << 4) | code;
                                }

                                token.value.push_back(c);

                                return false;
                            }
                            else
                            {
                                Log(Log::Level::ERR) << "Unrecognized escape character";
                                return false;
                            }
                        }
                        else if (isControlChar(*iterator))
                        {
                            Log(Log::Level::ERR) << "Unterminated string literal";
                            return false;
                        }
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
                    {
                        Log(Log::Level::ERR) << "Unknown keyword";
                        return false;
                    }
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
                {
                    Log(Log::Level::ERR) << "Unknown character";
                    return false;
                }

                tokens.push_back(token);
            }

            return true;
        }

        bool Data::init(const std::vector<uint8_t>& data)
        {
            std::vector<Token> tokens;

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

            if (!tokenize(str, tokens))
                return false;

            std::vector<Token>::const_iterator iterator = tokens.cbegin();

            return parseValue(tokens, iterator);
        }

        bool Data::save(const std::string& filename) const
        {
            std::vector<uint8_t> data;
            if (!encode(data)) return false;

            engine->getFileSystem()->writeFile(filename, data);

            return true;
        }

        bool Data::encode(std::vector<uint8_t>& data) const
        {
            data.clear();

            if (bom) data.insert(data.end(), {0xEF, 0xBB, 0xBF});

            return encodeValue(data);
        }
    } // namespace json
} // namespace ouzel
