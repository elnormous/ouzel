// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "JSON.hpp"
#include "core/Engine.hpp"
#include "Log.hpp"
#include "Utils.hpp"

namespace ouzel
{
    namespace json
    {
        Value::Value()
        {
        }

        bool Value::parseValue(const std::vector<Token>& tokens,
                               std::vector<Token>::iterator& iterator)
        {
            if (iterator == tokens.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of file";
                return false;
            }

            if (iterator->type == Token::Type::LEFT_BRACE)
            {
                return parseObject(tokens, iterator);
            }
            else if (iterator->type == Token::Type::LEFT_BRACKET)
            {
                return parseArray(tokens, iterator);
            }
            else if (iterator->type == Token::Type::LITERAL_NUMBER)
            {
                type = Type::NUMBER;
                doubleValue = std::stod(utf32to8(iterator->value));
                ++iterator;
            }
            else if (iterator->type == Token::Type::OPERATOR_MINUS)
            {
                ++iterator;

                if (iterator == tokens.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of file";
                    return false;
                }

                if (iterator->type != Token::Type::LITERAL_NUMBER)
                {
                    Log(Log::Level::ERR) << "Expected a number";
                    return false;
                }

                type = Type::NUMBER;
                doubleValue = -std::stod(utf32to8(iterator->value));
                ++iterator;
            }
            else if (iterator->type == Token::Type::LITERAL_STRING)
            {
                type = Type::STRING;
                stringValue = utf32to8(iterator->value);
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
                                std::vector<Token>::iterator& iterator)
        {
            if (iterator == tokens.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of file";
                return false;
            }

            if (iterator->type != Token::Type::LEFT_BRACE)
            {
                Log(Log::Level::ERR) << "Expected a left brace";
                return false;
            }

            if (++iterator == tokens.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of file";
                return false;
            }

            bool first = true;

            while (iterator->type != Token::Type::RIGHT_BRACE)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    if (iterator->type != Token::Type::COMMA)
                    {
                        Log(Log::Level::ERR) << "Expected a comma";
                        return false;
                    }

                    if (++iterator == tokens.end())
                    {
                        Log(Log::Level::ERR) << "Unexpected end of file";
                        return false;
                    }
                }

                if (iterator->type != Token::Type::LITERAL_STRING)
                {
                    Log(Log::Level::ERR) << "Expected a string literal";
                    return false;
                }

                std::string key = utf32to8(iterator->value);

                if (objectValue.find(key) != objectValue.end())
                {
                    Log(Log::Level::ERR) << "Duplicate key value";
                    return false;
                }

                if (++iterator == tokens.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of file";
                    return false;
                }

                if (iterator->type != Token::Type::COLON)
                {
                    Log(Log::Level::ERR) << "Expected a colon";
                    return false;
                }

                if (++iterator == tokens.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of file";
                    return false;
                }

                Value value;
                if (!value.parseValue(tokens, iterator))
                {
                    return false;
                }

                if (iterator == tokens.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of file";
                    return false;
                }

                objectValue[key] = value;
            }

            ++iterator;

            type = Type::OBJECT;

            return true;
        }

        bool Value::parseArray(const std::vector<Token>& tokens,
                               std::vector<Token>::iterator& iterator)
        {
            if (iterator == tokens.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of file";
                return false;
            }

            if (iterator->type != Token::Type::LEFT_BRACKET)
            {
                Log(Log::Level::ERR) << "Expected a left brace";
                return false;
            }

            if (++iterator == tokens.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of file";
                return false;
            }

            bool first = true;

            while (iterator->type != Token::Type::RIGHT_BRACKET)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    if (iterator->type != Token::Type::COMMA)
                    {
                        Log(Log::Level::ERR) << "Expected a comma";
                        return false;
                    }

                    if (++iterator == tokens.end())
                    {
                        Log(Log::Level::ERR) << "Unexpected end of file";
                        return false;
                    }
                }

                Value value;
                if (!value.parseValue(tokens, iterator))
                {
                    return false;
                }

                if (iterator == tokens.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of file";
                    return false;
                }

                arrayValue.push_back(value);
            }

            ++iterator;

            type = Type::ARRAY;

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
                    data.insert(data.end(), stringValue.begin(), stringValue.end());
                    break;
                case Type::OBJECT:
                {
                    data.push_back('{');
                    
                    bool first = true;
                    
                    for (const auto& value : objectValue)
                    {
                        if (first) first = false;
                        else data.push_back(',');
                        
                        data.insert(data.end(), value.first.begin(), value.first.end());
                        data.push_back(':');
                        value.second.encodeValue(data);
                    }
                    
                    data.push_back('}');
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
        
        Data::Data()
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

            if (!engine->getFileSystem()->readFile(filename, data))
            {
                return false;
            }

            return init(data);
        }

        bool Data::init(const std::vector<uint8_t>& data)
        {
            std::vector<Token> tokens;

            if (!tokenize(data, tokens))
            {
                return false;
            }

            std::vector<Token>::iterator iterator = tokens.begin();

            return parseValue(tokens, iterator);
        }

        bool Data::save(const std::string& filename) const
        {
            std::vector<uint8_t> data;
            
            if (!encode(data)) return false;
            
            return engine->getFileSystem()->writeFile(filename, data);
        }
        
        bool Data::encode(std::vector<uint8_t>& data) const
        {
            data.clear();

            if (bom) data.insert(data.end(), {0xEF, 0xBB, 0xBF});

            return encodeValue(data);
        }
        
        bool Data::tokenize(const std::vector<uint8_t>& data, std::vector<Token>& tokens)
        {
            tokens.clear();

            std::vector<uint32_t> utf32;

            // BOM
            if (data.size() >= 3 &&
                data[0] == 0xEF &&
                data[1] == 0xBB &&
                data[2] == 0xBF)
            {
                bom = true;
                utf32 = utf8to32(std::vector<uint8_t>(data.begin() + 3, data.end()));
            }
            else
            {
                bom = false;
                utf32 = utf8to32(data);
            }

            static const std::map<std::vector<uint32_t>, Token::Type> keywordMap = {
                {{0x74, 0x72, 0x75, 0x65}, Token::Type::KEYWORD_TRUE},
                {{0x66, 0x61, 0x6c, 0x73, 0x65}, Token::Type::KEYWORD_FALSE},
                {{0x6e, 0x75, 0x6c, 0x6c}, Token::Type::KEYWORD_NULL}
            };

            // tokenize
            for (std::vector<uint32_t>::const_iterator i = utf32.begin(); i != utf32.end();)
            {
                Token token;

                if (*i == '{' || *i == '}' ||
                    *i == '[' || *i == ']' ||
                    *i == ',' || *i == ':') // punctuation
                {
                    if (*i == '{') token.type = Token::Type::LEFT_BRACE;
                    if (*i == '}') token.type = Token::Type::RIGHT_BRACE;
                    if (*i == '[') token.type = Token::Type::LEFT_BRACKET;
                    if (*i == ']') token.type = Token::Type::RIGHT_BRACKET;
                    if (*i == ',') token.type = Token::Type::COMMA;
                    if (*i == ':') token.type = Token::Type::COLON;
                    token.value.push_back(*i);

                    ++i;
                }
                else if ((*i >= '0' && *i <= '9') ||  // number
                         (*i == '.' && (i + 1) != utf32.end() && *(i + 1) >= '0' && *(i + 1) <= '9')) // starts with a dot
                {
                    token.type = Token::Type::LITERAL_NUMBER;

                    while (i != utf32.end() &&
                           ((*i >= '0' && *i <= '9')))
                    {
                        token.value.push_back(*i);
                        ++i;
                    }

                    if (i != utf32.end() && *i == '.')
                    {
                        token.value.push_back(*i);
                        ++i;

                        while (i != utf32.end() &&
                               ((*i >= '0' && *i <= '9')))
                        {
                            token.value.push_back(*i);
                            ++i;
                        }
                    }

                    // parse exponent
                    if (i != utf32.end() &&
                        (*i == 'e' || *i == 'E'))
                    {
                        token.value.push_back(*i);
                        ++i;

                        if (i == utf32.end() || *i != '+' || *i != '-')
                        {
                            Log(Log::Level::ERR) << "Invalid exponent";
                            return false;
                        }

                        token.value.push_back(*i);
                        ++i;

                        if (i == utf32.end() || *i < '0' || *i > '9')
                        {
                            Log(Log::Level::ERR) << "Invalid exponent";
                            return false;
                        }

                        while (i != utf32.end() &&
                               ((*i >= '0' && *i <= '9')))
                        {
                            token.value.push_back(*i);
                            ++i;
                        }
                    }
                }
                else if (*i == '"') // string literal
                {
                    token.type = Token::Type::LITERAL_STRING;

                    while (++i != utf32.end() &&
                           *i != '"')
                    {
                        if (*i == '\\')
                        {
                            if (++i == utf32.end())
                            {
                                Log(Log::Level::ERR) << "Unterminated string literal";
                                return false;
                            }

                            if (*i == 'a') token.value.push_back('\a');
                            else if (*i == 'b') token.value.push_back('\b');
                            else if (*i == 't') token.value.push_back('\t');
                            else if (*i == 'n') token.value.push_back('\n');
                            else if (*i == 'f') token.value.push_back('\f');
                            else if (*i == 'r') token.value.push_back('\r');
                            else if (*i == '"') token.value.push_back('"');
                            else if (*i == '\?') token.value.push_back('\?');
                            else if (*i == '\\') token.value.push_back('\\');
                            else
                            {
                                Log(Log::Level::ERR) << "Unrecognized escape character";
                                return false;
                            }
                            // TODO: handle numeric character references
                        }
                        else if (*i == '\n' || *i == '\r')
                        {
                            Log(Log::Level::ERR) << "Unterminated string literal";
                            return false;
                        }
                        else
                        {
                            token.value.push_back(*i);
                        }
                    }

                    if (*i == '"')
                    {
                        ++i;
                    }
                    else
                    {
                        Log(Log::Level::ERR) << "Unterminated string literal";
                        return false;
                    }
                }
                else if ((*i >= 'a' && *i <= 'z') ||
                         (*i >= 'A' && *i <= 'Z') ||
                         *i == '_')
                {
                    while (i != utf32.end() &&
                           ((*i >= 'a' && *i <= 'z') ||
                            (*i >= 'A' && *i <= 'Z') ||
                            *i == '_' ||
                            (*i >= '0' && *i <= '9')))
                    {
                        token.value.push_back(*i);
                        ++i;
                    }
                    
                    std::map<std::vector<uint32_t>, Token::Type>::const_iterator keywordIterator;
                    
                    if ((keywordIterator = keywordMap.find(token.value)) != keywordMap.end())
                    {
                        token.type = keywordIterator->second;
                    }
                    else
                    {
                        Log(Log::Level::ERR) << "Unknown keyword";
                        return false;
                    }
                }
                else if (*i == '-')
                {
                    token.type = Token::Type::OPERATOR_MINUS;
                    token.value.push_back(*i);
                    ++i;
                }
                else if (*i == ' ' || *i == '\t' || *i == '\n' || *i == '\r') // whitespace
                {
                    ++i;
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
    } // namespace json
}
