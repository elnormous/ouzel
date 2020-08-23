// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_FORMATS_JSON_HPP
#define OUZEL_FORMATS_JSON_HPP

#include <algorithm>
#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace ouzel::json
{
    class ParseError final: public std::logic_error
    {
    public:
        explicit ParseError(const std::string& str): std::logic_error(str) {}
        explicit ParseError(const char* str): std::logic_error(str) {}
    };

    class TypeError final: public std::runtime_error
    {
    public:
        explicit TypeError(const std::string& str): std::runtime_error(str) {}
        explicit TypeError(const char* str): std::runtime_error(str) {}
    };

    class RangeError final: public std::runtime_error
    {
    public:
        explicit RangeError(const std::string& str): std::runtime_error(str) {}
        explicit RangeError(const char* str): std::runtime_error(str) {}
    };

    class Value final
    {
    public:
        using Array = std::vector<Value>;
        using Object = std::map<std::string, Value>;

        enum class Type
        {
            null,
            integer,
            floatingPoint,
            string,
            object,
            array,
            boolean
        };

        Value() = default;

        Value(const Type initType): type(initType) {}

        template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
        Value(const T value): type(Type::floatingPoint), doubleValue(std::isfinite(value) ? static_cast<double>(value) : 0.0) {}

        template <typename T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
        Value(const T value): type(Type::integer), intValue(static_cast<std::int64_t>(value)) {}

        Value(const std::string& value): type(Type::string), stringValue(value) {}

        Value(const char* value): type(Type::string), stringValue(value) {}

        Value(const bool value): type(Type::boolean), boolValue(value) {}

        Value(const std::nullptr_t): type(Type::null) {}

        Value(const Array& value): type(Type::array), arrayValue(value) {}

        Value(const Object& value): type(Type::object), objectValue(value) {}

        Value& operator=(const Type newType) noexcept
        {
            type = newType;
            return *this;
        }

        template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
        Value& operator=(const T value) noexcept
        {
            type = Type::floatingPoint;
            doubleValue = std::isfinite(value) ? static_cast<double>(value) : 0.0;
            return *this;
        }

        template <typename T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
        Value& operator=(const T value) noexcept
        {
            type = Type::integer;
            intValue = static_cast<std::int64_t>(value);
            return *this;
        }

        Value& operator=(const std::string& value)
        {
            type = Type::string;
            stringValue = value;
            return *this;
        }

        Value& operator=(const char* value)
        {
            type = Type::string;
            stringValue = value;
            return *this;
        }

        Value& operator=(const bool value) noexcept
        {
            type = Type::boolean;
            boolValue = value;
            return *this;
        }

        Value& operator=(std::nullptr_t) noexcept
        {
            type = Type::null;
            objectValue.clear();
            return *this;
        }

        Value& operator=(const Array& value)
        {
            type = Type::array;
            arrayValue = value;
            return *this;
        }

        Value& operator=(const Object& value)
        {
            type = Type::object;
            objectValue = value;
            return *this;
        }

        Type getType() const noexcept { return type; }

        template <typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
        std::string& as() noexcept
        {
            type = Type::string;
            return stringValue;
        }

        template <typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
        const std::string& as() const
        {
            if (type != Type::string) throw TypeError("Wrong type");
            return stringValue;
        }

        template <typename T, typename std::enable_if<std::is_same<T, const char*>::value>::type* = nullptr>
        T as() const
        {
            if (type != Type::string) throw TypeError("Wrong type");
            return stringValue.c_str();
        }

        template <typename T, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
        T as() const
        {
            if (type != Type::boolean && type != Type::integer && type != Type::floatingPoint)
                throw TypeError("Wrong type");
            if (type == Type::boolean) return boolValue;
            else if (type == Type::integer) return intValue != 0;
            else return doubleValue != 0.0;
        }

        template <typename T, typename std::enable_if<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
        T as() const
        {
            if (type != Type::boolean && type != Type::integer && type != Type::floatingPoint)
                throw TypeError("Wrong type");
            if (type == Type::boolean) return boolValue;
            else if (type == Type::integer) return static_cast<T>(intValue);
            else return static_cast<T>(doubleValue);
        }

        template <typename T, typename std::enable_if<std::is_same<T, Object>::value>::type* = nullptr>
        T& as() noexcept
        {
            type = Type::object;
            return objectValue;
        }

        template <typename T, typename std::enable_if<std::is_same<T, Object>::value>::type* = nullptr>
        const T& as() const
        {
            if (type != Type::object) throw TypeError("Wrong type");
            return objectValue;
        }

        template <typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
        T& as() noexcept
        {
            type = Type::array;
            return arrayValue;
        }

        template <typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
        const T& as() const
        {
            if (type != Type::array) throw TypeError("Wrong type");
            return arrayValue;
        }

        Array::iterator begin()
        {
            if (type != Type::array) throw TypeError("Wrong type");
            return arrayValue.begin();
        }

        Array::iterator end()
        {
            if (type != Type::array) throw TypeError("Wrong type");
            return arrayValue.end();
        }

        Array::const_iterator begin() const
        {
            if (type != Type::array) throw TypeError("Wrong type");
            return arrayValue.begin();
        }

        Array::const_iterator end() const
        {
            if (type != Type::array) throw TypeError("Wrong type");
            return arrayValue.end();
        }

        bool isNull() const noexcept
        {
            return type == Type::null;
        }

        bool hasMember(const std::string& member) const
        {
            if (type != Type::object) throw TypeError("Wrong type");
            return objectValue.find(member) != objectValue.end();
        }

        Value& operator[](const std::string& member)
        {
            type = Type::object;
            return objectValue[member];
        }

        const Value& operator[](const std::string& member) const
        {
            if (type != Type::object) throw TypeError("Wrong type");

            auto i = objectValue.find(member);
            if (i != objectValue.end())
                return i->second;
            else
                throw RangeError("Member does not exist");
        }

        Value& operator[](std::size_t index)
        {
            type = Type::array;
            if (index >= arrayValue.size()) arrayValue.resize(index + 1);
            return arrayValue[index];
        }

        const Value& operator[](std::size_t index) const
        {
            if (type != Type::array) throw TypeError("Wrong type");

            if (index < arrayValue.size())
                return arrayValue[index];
            else
                throw RangeError("Index out of range");
        }

        std::size_t getSize() const
        {
            if (type != Type::array) throw TypeError("Wrong type");
            return arrayValue.size();
        }

        void resize(std::size_t size)
        {
            if (type != Type::array) throw TypeError("Wrong type");
            arrayValue.resize(size);
        }

        void pushBack(const Value& value)
        {
            if (type != Type::array) throw TypeError("Wrong type");
            arrayValue.push_back(value);
        }

    private:
        Type type = Type::object;
        union
        {
            bool boolValue = false;
            std::int64_t intValue;
            double doubleValue;
        };
        Object objectValue;
        Array arrayValue;
        std::string stringValue;
    };

    inline namespace detail
    {
        constexpr std::uint8_t utf8ByteOrderMark[] = {0xEF, 0xBB, 0xBF};
    }

    template <class Iterator>
    Value parse(Iterator begin, Iterator end)
    {
        class Parser final
        {
        public:
            static Value parse(Iterator begin, Iterator end)
            {
                auto iterator = hasByteOrderMark(begin, end) ? begin + 3 : begin;
                Value result;
                std::tie(result, iterator) = parseValue(iterator, end);

                iterator = skipWhitespaces(iterator, end);

                if (iterator != end)
                    throw ParseError("Unexpected data");

                return result;
            }

        private:
            static bool hasByteOrderMark(Iterator begin, Iterator end) noexcept
            {
                for (auto i = std::begin(utf8ByteOrderMark); i != std::end(utf8ByteOrderMark); ++i)
                    if (begin == end || static_cast<std::uint8_t>(*begin) != *i)
                        return false;
                    else
                        ++begin;
                return true;
            }

            static Iterator skipWhitespaces(Iterator begin, Iterator end)
            {
                Iterator iterator = begin;
                while (iterator != end)
                {
                    if (static_cast<char>(*iterator) != ' ' &&
                        static_cast<char>(*iterator) != '\t' &&
                        static_cast<char>(*iterator) != '\r' &&
                        static_cast<char>(*iterator) != '\n')
                        break;

                    ++iterator;
                }
                return iterator;
            }

            static std::pair<bool, Iterator> isSame(Iterator begin, Iterator end,
                                                    const char* expectedBegin, const char* expectedEnd)
            {
                auto iterator = begin;

                while (iterator != end && expectedBegin != expectedEnd)
                {
                    if (static_cast<char>(*iterator) != *expectedBegin)
                        return std::make_pair(false, iterator);

                    ++iterator;
                    ++expectedBegin;
                }

                return std::make_pair(true, iterator);
            }

            static std::pair<Value, Iterator> parseValue(Iterator begin, Iterator end)
            {
                Iterator iterator = skipWhitespaces(begin, end);

                if (iterator == end)
                    throw ParseError("Unexpected end of data");

                if (static_cast<char>(*iterator) == '{')
                {
                    ++iterator;

                    Value result = Value::Type::object;

                    bool firstValue = true;

                    while ((iterator = skipWhitespaces(iterator, end)) != end &&
                           static_cast<char>(*iterator) != '}')
                    {
                        if (firstValue)
                            firstValue = false;
                        else
                        {
                            if (static_cast<char>(*iterator++) != ',')
                                throw ParseError("Invalid object");

                            iterator = skipWhitespaces(iterator, end);
                        }

                        std::string key;
                        std::tie(key, iterator) = parseString(iterator, end);

                        iterator = skipWhitespaces(iterator, end);

                        if (static_cast<char>(*iterator++) != ':')
                            throw ParseError("Invalid object");

                        iterator = skipWhitespaces(iterator, end);

                        std::tie(result[key], iterator) = parseValue(iterator, end);
                    }

                    if (iterator == end || static_cast<char>(*iterator++) != '}')
                        throw ParseError("Invalid object");

                    return std::make_pair(result, iterator);
                }
                else if (static_cast<char>(*iterator) == '[')
                {
                    ++iterator;

                    Value result = Value::Type::array;

                    bool firstValue = true;

                    while ((iterator = skipWhitespaces(iterator, end)) != end &&
                           static_cast<char>(*iterator) != ']')
                    {
                        if (firstValue)
                            firstValue = false;
                        else
                        {
                            if (static_cast<char>(*iterator++) != ',')
                                throw ParseError("Invalid object");

                            iterator = skipWhitespaces(iterator, end);
                        }

                        Value value;
                        std::tie(value, iterator) = parseValue(iterator, end);
                        result.pushBack(value);
                    }

                    if (iterator == end || static_cast<char>(*iterator++) != ']')
                        throw ParseError("Invalid array");

                    return std::make_pair(result, iterator);
                }
                else if (static_cast<char>(*iterator) == '-' ||
                         (static_cast<char>(*iterator) >= '0' &&
                          static_cast<char>(*iterator) <= '9'))
                {
                    std::string value;

                    if (static_cast<char>(*iterator) == '-')
                    {
                        value.push_back('-');

                        if (++iterator == end ||
                            static_cast<char>(*iterator) < '0' ||
                            static_cast<char>(*iterator) > '9')
                            throw ParseError("Invalid number");
                    }

                    while (iterator != end &&
                           (static_cast<char>(*iterator) >= '0' &&
                            static_cast<char>(*iterator) <= '9'))
                    {
                        value.push_back(static_cast<char>(*iterator));
                        ++iterator;
                    }

                    if (iterator != end &&
                        static_cast<char>(*iterator) == '.')
                    {
                        value.push_back(static_cast<char>(*iterator));
                        ++iterator;

                        while (iterator != end &&
                               (static_cast<char>(*iterator) >= '0' &&
                                static_cast<char>(*iterator) <= '9'))
                        {
                            value.push_back(static_cast<char>(*iterator));
                            ++iterator;
                        }
                    }
                    else
                        return std::make_pair(Value{std::stoll(value)}, iterator);

                    // parse exponent
                    if (iterator != end &&
                        (static_cast<char>(*iterator) == 'e' ||
                         static_cast<char>(*iterator) == 'E'))
                    {
                        value.push_back(static_cast<char>(*iterator));

                        if (++iterator == end)
                            throw ParseError("Invalid exponent");

                        if (static_cast<char>(*iterator) == '+' ||
                            static_cast<char>(*iterator) == '-')
                            value.push_back(static_cast<char>(*iterator++));

                        if (iterator == end ||
                            static_cast<char>(*iterator) < '0' ||
                            static_cast<char>(*iterator) > '9')
                            throw ParseError("Invalid exponent");

                        while (iterator != end &&
                               static_cast<char>(*iterator) >= '0' &&
                               static_cast<char>(*iterator) <= '9')
                        {
                            value.push_back(static_cast<char>(*iterator));
                            ++iterator;
                        }
                    }

                    return std::make_pair(Value{std::stod(value)}, iterator);
                }
                else if (static_cast<char>(*iterator) == '"')
                {
                    std::string stringValue;
                    std::tie(stringValue, iterator) = parseString(iterator, end);
                    return std::make_pair(Value{stringValue}, iterator);
                }
                else
                {
                    constexpr char trueString[] = {'t', 'r', 'u', 'e'};
                    constexpr char falseString[] = {'f', 'a', 'l', 's', 'e'};
                    constexpr char nullString[] = {'n', 'u', 'l', 'l'};

                    bool isTrue;
                    std::tie(isTrue, iterator) = isSame(iterator, end, std::begin(trueString), std::end(trueString));
                    if (isTrue)
                        return std::make_pair(Value{true}, iterator);

                    bool isFalse;
                    std::tie(isFalse, iterator) = isSame(iterator, end, std::begin(falseString), std::end(falseString));
                    if (isFalse)
                        return std::make_pair(Value{false}, iterator);

                    bool isNull;
                    std::tie(isNull, iterator) = isSame(iterator, end, std::begin(nullString), std::end(nullString));
                    if (isNull)
                        return std::make_pair(Value{nullptr}, iterator);

                    throw ParseError("Unexpected identifier");
                }
            }

            static std::pair<std::string, Iterator> parseString(Iterator begin, Iterator end)
            {
                std::string result;
                Iterator iterator = begin;

                if (iterator == end || static_cast<char>(*iterator) != '"')
                    throw ParseError("Invalid string");

                ++iterator;

                while (iterator != end && static_cast<char>(*iterator) != '"')
                {
                    if (static_cast<char>(*iterator) == '\\')
                    {
                        if (++iterator == end)
                            throw ParseError("Unterminated string literal");

                        switch (static_cast<char>(*iterator))
                        {
                            case '"': result.push_back('"'); break;
                            case '\\': result.push_back('\\'); break;
                            case '/': result.push_back('/'); break;
                            case 'b': result.push_back('\b'); break;
                            case 'f': result.push_back('\f'); break;
                            case 'n': result.push_back('\n'); break;
                            case 'r': result.push_back('\r'); break;
                            case 't': result.push_back('\t'); break;
                            case 'u':
                            {
                                char32_t c = 0;

                                for (std::uint32_t i = 0; i < 4; ++i)
                                {
                                    if (iterator == end)
                                        throw ParseError("Unexpected end of data");

                                    std::uint8_t code = 0;

                                    if (static_cast<char>(*iterator) >= '0' && static_cast<char>(*iterator) <= '9')
                                        code = static_cast<std::uint8_t>(*iterator) - '0';
                                    else if (static_cast<char>(*iterator) >= 'a' && static_cast<char>(*iterator) <='f')
                                        code = static_cast<std::uint8_t>(*iterator) - 'a' + 10;
                                    else if (static_cast<char>(*iterator) >= 'A' && static_cast<char>(*iterator) <='F')
                                        code = static_cast<std::uint8_t>(*iterator) - 'A' + 10;
                                    else
                                        throw ParseError("Invalid character code");

                                    c = (c << 4) | code;

                                    ++iterator;
                                }

                                if (c <= 0x7F)
                                    result.push_back(static_cast<char>(c));
                                else if (c <= 0x7FF)
                                {
                                    result.push_back(static_cast<char>(0xC0 | ((c >> 6) & 0x1F)));
                                    result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                                }
                                else if (c <= 0xFFFF)
                                {
                                    result.push_back(static_cast<char>(0xE0 | ((c >> 12) & 0x0F)));
                                    result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                                    result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                                }
                                else
                                {
                                    result.push_back(static_cast<char>(0xF0 | ((c >> 18) & 0x07)));
                                    result.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
                                    result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                                    result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                                }

                                break;
                            }
                            default:
                                throw ParseError("Unrecognized escape character");
                        }
                    }
                    else if (static_cast<std::uint8_t>(*iterator) <= 0x1F) // control char
                        throw ParseError("Unterminated string literal");
                    else
                        result.push_back(static_cast<char>(*iterator));

                    ++iterator;
                }

                if (iterator == end || static_cast<char>(*iterator++) != '"')
                    throw ParseError("Invalid string");

                return std::make_pair(result, iterator);
            }
        };

        return Parser::parse(begin, end);
    }

    inline Value parse(const char* data)
    {
        const char* end = data;
        while (*end) ++end;
        return parse(data, end);
    }

    template <class Data>
    Value parse(const Data& data)
    {
        return parse(std::begin(data), std::end(data));
    }

    inline std::string encode(const Value& value, bool whitespaces = false, bool byteOrderMark = false)
    {
        class Encoder final
        {
        public:
            static std::string encode(const Value& value, bool whitespaces, bool byteOrderMark)
            {
                std::string result;
                if (byteOrderMark) result.assign(std::begin(utf8ByteOrderMark),
                                                 std::end(utf8ByteOrderMark));
                encode(value, result, whitespaces);
                return result;
            }

        private:
            static void encode(const std::string& str, std::string& result)
            {
                for (const auto c : str)
                    if (c == '"') result.insert(result.end(), {'\\', '"'});
                    else if (c == '\\') result.insert(result.end(), {'\\', '\\'});
                    else if (c == '/') result.insert(result.end(), {'\\', '/'});
                    else if (c == '\b') result.insert(result.end(), {'\\', 'b'});
                    else if (c == '\f') result.insert(result.end(), {'\\', 'f'});
                    else if (c == '\n') result.insert(result.end(), {'\\', 'n'});
                    else if (c == '\r') result.insert(result.end(), {'\\', 'r'});
                    else if (c == '\t') result.insert(result.end(), {'\\', 't'});
                    else if (c <= 0x1F)
                    {
                        result.insert(result.end(), {'\\', 'u'});

                        constexpr char digits[] = "0123456789abcdef";
                        for (std::uint32_t p = 0; p < 4; ++p)
                            result.push_back(digits[(c >> (12 - p * 4)) & 0x0F]);
                    }
                    else
                        result.push_back(c);
            }

            static void encode(const Value& value, std::string& result, bool whitespaces, size_t level = 0)
            {
                switch (value.getType())
                {
                    case Value::Type::null:
                    {
                        result.insert(result.end(), {'n', 'u', 'l', 'l'});
                        break;
                    }
                    case Value::Type::integer:
                    {
                        const auto str = std::to_string(value.as<std::int64_t>());
                        result.insert(result.end(), str.begin(), str.end());
                        break;
                    }
                    case Value::Type::floatingPoint:
                    {
                        const auto str = std::to_string(value.as<double>());
                        result.insert(result.end(), str.begin(), str.end());
                        break;
                    }
                    case Value::Type::string:
                        result.push_back('"');
                        encode(value.as<std::string>(), result);
                        result.push_back('"');
                        break;
                    case Value::Type::object:
                    {
                        result.push_back('{');
                        if (whitespaces) result.push_back('\n');

                        const auto& entries = value.as<Value::Object>();

                        for (auto entryIterator = entries.begin(); entryIterator != entries.end();)
                        {
                            const auto& entry = *entryIterator;
                            if (whitespaces) result.insert(result.end(), level + 1, '\t');
                            result.push_back('"');
                            encode(entry.first, result);
                            result.insert(result.end(), {'"', ':'});
                            encode(entry.second, result, whitespaces, level + 1);
                            if (++entryIterator != entries.end()) result.push_back(',');
                            if (whitespaces) result.push_back('\n');
                        }

                        if (whitespaces) result.insert(result.end(), level, '\t');
                        result.push_back('}');
                        break;
                    }
                    case Value::Type::array:
                    {
                        result.push_back('[');
                        if (whitespaces) result.push_back('\n');

                        const auto& entries = value.as<Value::Array>();

                        for (auto entryIterator = entries.begin(); entryIterator != entries.end();)
                        {
                            const auto& entry = *entryIterator;
                            if (whitespaces) result.insert(result.end(), level + 1, '\t');
                            encode(entry, result, whitespaces, level + 1);
                            if (++entryIterator != entries.end()) result.push_back(',');
                            if (whitespaces) result.push_back('\n');
                        }

                        if (whitespaces) result.insert(result.end(), level, '\t');
                        result.push_back(']');
                        break;
                    }
                    case Value::Type::boolean:
                        if (value.as<bool>()) result.insert(result.end(), {'t', 'r', 'u', 'e'});
                        else result.insert(result.end(), {'f', 'a', 'l', 's', 'e'});
                        break;
                    default:
                        throw ParseError("Unknown value type");
                }
            }
        };

        return Encoder::encode(value, whitespaces, byteOrderMark);
    }
}

#endif // OUZEL_FORMATS_JSON_HPP
