// Ouzel by Elviss Strazdins

#ifndef OUZEL_FORMATS_JSON_HPP
#define OUZEL_FORMATS_JSON_HPP

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

namespace ouzel::json
{
    class ParseError final: public std::logic_error
    {
    public:
        using std::logic_error::logic_error;
    };

    class TypeError final: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    class RangeError final: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    class Value final
    {
    public:
        using Array = std::vector<Value>;
        using Object = std::map<std::string, Value, std::less<>>;
        using String = std::string;

        Value() noexcept = default;

        template <typename T, typename std::enable_if_t<
            std::is_same_v<T, std::nullptr_t> ||
            std::is_same_v<T, bool>
        >* = nullptr>
        Value(const T v) noexcept: value{v} {}

        template <typename T, typename std::enable_if_t<
            std::is_arithmetic_v<T> &&
            !std::is_same_v<T, bool>
        >* = nullptr>
        Value(const T value): value{static_cast<double>(value)} {}

        template <typename T, typename std::enable_if_t<
            std::is_same_v<T, Array> ||
            std::is_same_v<T, Object> ||
            std::is_same_v<T, String>
        >* = nullptr>
        Value(const T& v): value{v} {}

        template <typename T, typename std::enable_if_t<
            std::is_same_v<T, char*> ||
            std::is_same_v<T, const char*>
        >* = nullptr>
        Value(const T v): value{String{v}} {}

        template <typename T, typename std::enable_if_t<
            std::is_same_v<T, std::nullptr_t> ||
            std::is_same_v<T, bool>
        >* = nullptr>
        Value& operator=(const T v) noexcept
        {
            value = v;
            return *this;
        }

        template <typename T, typename std::enable_if_t<
            std::is_arithmetic_v<T> &&
            !std::is_same_v<T, bool>
        >* = nullptr>
        Value& operator=(const T v) noexcept
        {
            value = static_cast<double>(v);
            return *this;
        }

        template <typename T, typename std::enable_if_t<
            std::is_same_v<T, Array> ||
            std::is_same_v<T, Object> ||
            std::is_same_v<T, String>
        >* = nullptr>
        Value& operator=(T&& v)
        {
            value = std::forward<T>(v);
            return *this;
        }

        template <typename T, typename std::enable_if_t<
            std::is_same_v<T, char*> ||
            std::is_same_v<T, const char*>
        >* = nullptr>
        Value& operator=(const T v)
        {
            value = String{v};
            return *this;
        }

        template <typename T>
        bool is() const noexcept
        {
            return std::holds_alternative<T>(value);
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, bool>>* = nullptr>
        T as() const
        {
            if (const auto b = std::get_if<bool>(&value))
                return *b;
            else if (const auto d = std::get_if<double>(&value))
                return *d != 0.0;
            else
                throw TypeError{"Wrong type"};
        }

        template <typename T, typename std::enable_if_t<
            std::is_arithmetic_v<T> &&
            !std::is_same_v<T, bool>
        >* = nullptr>
        T as() const
        {
            if (const auto d = std::get_if<double>(&value))
                return static_cast<T>(*d);
            else if (const auto b = std::get_if<bool>(&value))
                return *b ? T(1.0) : T(0.0);
            else
                throw TypeError{"Wrong type"};
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Array>>* = nullptr>
        T& as()
        {
            if (const auto p = std::get_if<Array>(&value))
                return *p;
            else
                throw TypeError{"Wrong type"};
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Array>>* = nullptr>
        const T& as() const
        {
            if (const auto p = std::get_if<Array>(&value))
                return *p;
            else
                throw TypeError{"Wrong type"};
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Object>>* = nullptr>
        T& as()
        {
            if (const auto p = std::get_if<Object>(&value))
                return *p;
            else
                throw TypeError{"Wrong type"};
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Object>>* = nullptr>
        const T& as() const
        {
            if (const auto p = std::get_if<Object>(&value))
                return *p;
            else
                throw TypeError{"Wrong type"};
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, String>>* = nullptr>
        String& as()
        {
            if (const auto p = std::get_if<String>(&value))
                return *p;
            else
                throw TypeError{"Wrong type"};
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, String>>* = nullptr>
        const String& as() const
        {
            if (const auto p = std::get_if<String>(&value))
                return *p;
            else
                throw TypeError{"Wrong type"};
        }

        template <typename T, typename std::enable_if_t<
            std::is_same_v<T, char*> ||
            std::is_same_v<T, const char*>
        >* = nullptr>
        T as() const
        {
            if (const auto p = std::get_if<String>(&value))
                return p->c_str();
            else
                throw TypeError{"Wrong type"};
        }

        Array::iterator begin()
        {
            if (const auto p = std::get_if<Array>(&value))
                return p->begin();
            else
                throw TypeError{"Wrong type"};
        }

        Array::iterator end()
        {
            if (const auto p = std::get_if<Array>(&value))
                return p->end();
            else
                throw TypeError{"Wrong type"};
        }

        Array::const_iterator begin() const
        {
            if (const auto p = std::get_if<Array>(&value))
                return p->begin();
            else
                throw TypeError{"Wrong type"};
        }

        Array::const_iterator end() const
        {
            if (const auto p = std::get_if<Array>(&value))
                return p->end();
            else
                throw TypeError{"Wrong type"};
        }

        bool hasMember(std::string_view member) const
        {
            if (const auto p = std::get_if<Object>(&value))
                return p->find(member) != p->end();
            else
                throw TypeError{"Wrong type"};
        }

        Value& operator[](std::string_view member) &
        {
            if (const auto p = std::get_if<Object>(&value))
            {
                if (const auto iterator = p->find(member); iterator != p->end())
                    return iterator->second;
                else
                {
                    const auto [newIterator, success] = p->insert({std::string{member}, Value{}});
                    (void)success;
                    return newIterator->second;
                }
            }
            else
                throw TypeError{"Wrong type"};
        }

        const Value& operator[](std::string_view member) const&
        {
            if (const auto p = std::get_if<Object>(&value))
            {
                if (const auto iterator = p->find(member); iterator != p->end())
                    return iterator->second;
                else
                    throw RangeError{"Member does not exist"};
            }
            else
                throw TypeError{"Wrong type"};
        }

        Value& operator[](std::size_t index) &
        {
            if (const auto p = std::get_if<Array>(&value))
            {
                if (index >= p->size()) p->resize(index + 1);
                return (*p)[index];
            }
            else
                throw TypeError{"Wrong type"};
        }

        const Value& operator[](std::size_t index) const&
        {
            if (const auto p = std::get_if<Array>(&value))
            {
                if (index < p->size())
                    return (*p)[index];
                else
                    throw RangeError{"Index out of range"};
            }
            else
                throw TypeError{"Wrong type"};
        }

        bool isEmpty() const
        {
            if (const auto p = std::get_if<Array>(&value))
                return p->empty();
            else
                throw TypeError{"Wrong type"};
        }

        std::size_t getSize() const
        {
            if (const auto p = std::get_if<Array>(&value))
                return p->size();
            else
                throw TypeError{"Wrong type"};
        }

        void resize(std::size_t size) &
        {
            if (const auto p = std::get_if<Array>(&value))
                return p->resize(size);
            else
                throw TypeError{"Wrong type"};
        }

        void pushBack(const Value& v) &
        {
            if (const auto p = std::get_if<Array>(&value))
                return p->push_back(v);
            else
                throw TypeError{"Wrong type"};
        }

        auto& getValue() const noexcept { return value; }

    private:
        std::variant<std::nullptr_t, bool, double, Object, Array, String> value;
    };

    inline namespace detail
    {
        constexpr std::array<std::uint8_t, 3> utf8ByteOrderMark{0xEF, 0xBB, 0xBF};
    }

    template <class Iterator>
    Value parse(Iterator begin, Iterator end)
    {
        class Parser final
        {
        public:
            static Value parse(Iterator begin, Iterator end)
            {
                const auto startIterator = hasByteOrderMark(begin, end) ? begin + 3 : begin;
                const auto [result, valueIterator] = parseValue(startIterator, end);
                if (const auto endIterator = skipWhitespaces(valueIterator, end); endIterator != end)
                    throw ParseError{"Unexpected data"};

                return result;
            }

        private:
            static bool hasByteOrderMark(Iterator begin, Iterator end) noexcept
            {
                for (const auto b : utf8ByteOrderMark)
                    if (begin == end || static_cast<std::uint8_t>(*begin) != b)
                        return false;
                    else
                        ++begin;
                return true;
            }

            static constexpr bool isWhitespace(const char c) noexcept
            {
                return c == ' ' || c == '\t' || c == '\r' || c == '\n';
            }

            static Iterator skipWhitespaces(Iterator begin, Iterator end)
            {
                for (auto i = begin; i != end; ++i)
                    if (!isWhitespace(static_cast<char>(*i))) return i;
                return end;
            }

            static std::pair<bool, Iterator> isSame(Iterator begin, Iterator end,
                                                    const char* expectedBegin,
                                                    const char* expectedEnd)
            {
                auto iterator = begin;

                while (iterator != end && expectedBegin != expectedEnd)
                {
                    if (static_cast<char>(*iterator) != *expectedBegin)
                        return std::pair(false, iterator);

                    ++iterator;
                    ++expectedBegin;
                }

                return std::pair(true, iterator);
            }

            static std::pair<Value, Iterator> parseValue(Iterator begin, Iterator end)
            {
                Iterator iterator = skipWhitespaces(begin, end);

                if (iterator == end)
                    throw ParseError{"Unexpected end of data"};

                if (static_cast<char>(*iterator) == '{')
                {
                    ++iterator;

                    Value result = Value::Object{};

                    bool firstValue = true;

                    while ((iterator = skipWhitespaces(iterator, end)) != end &&
                           static_cast<char>(*iterator) != '}')
                    {
                        if (firstValue)
                            firstValue = false;
                        else
                        {
                            if (static_cast<char>(*iterator++) != ',')
                                throw ParseError{"Invalid object"};

                            iterator = skipWhitespaces(iterator, end);
                        }

                        const auto [key, stringIterator] = parseString(iterator, end);
                        iterator = skipWhitespaces(stringIterator, end);

                        if (static_cast<char>(*iterator++) != ':')
                            throw ParseError{"Invalid object"};

                        iterator = skipWhitespaces(iterator, end);

                        const auto [value, valueIterator] = parseValue(iterator, end);
                        iterator = valueIterator;
                        result[key] = std::move(value);
                    }

                    if (iterator == end || static_cast<char>(*iterator) != '}')
                        throw ParseError{"Invalid object"};

                    ++iterator;

                    return std::pair(result, iterator);
                }
                else if (static_cast<char>(*iterator) == '[')
                {
                    ++iterator;

                    Value result = Value::Array{};

                    bool firstValue = true;

                    while ((iterator = skipWhitespaces(iterator, end)) != end &&
                           static_cast<char>(*iterator) != ']')
                    {
                        if (firstValue)
                            firstValue = false;
                        else
                        {
                            if (static_cast<char>(*iterator++) != ',')
                                throw ParseError{"Invalid object"};

                            iterator = skipWhitespaces(iterator, end);
                        }

                        const auto [value, valueIterator] = parseValue(iterator, end);
                        iterator = valueIterator;
                        result.pushBack(value);
                    }

                    if (iterator == end || static_cast<char>(*iterator) != ']')
                        throw ParseError{"Invalid array"};

                    ++iterator;

                    return std::pair(result, iterator);
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
                            throw ParseError{"Invalid number"};
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
                        return std::pair(Value{std::stoll(value)}, iterator);

                    // parse exponent
                    if (iterator != end &&
                        (static_cast<char>(*iterator) == 'e' ||
                         static_cast<char>(*iterator) == 'E'))
                    {
                        value.push_back(static_cast<char>(*iterator));

                        if (++iterator == end)
                            throw ParseError{"Invalid exponent"};

                        if (static_cast<char>(*iterator) == '+' ||
                            static_cast<char>(*iterator) == '-')
                            value.push_back(static_cast<char>(*iterator++));

                        if (iterator == end ||
                            static_cast<char>(*iterator) < '0' ||
                            static_cast<char>(*iterator) > '9')
                            throw ParseError{"Invalid exponent"};

                        while (iterator != end &&
                               static_cast<char>(*iterator) >= '0' &&
                               static_cast<char>(*iterator) <= '9')
                        {
                            value.push_back(static_cast<char>(*iterator));
                            ++iterator;
                        }
                    }

                    return std::pair(Value{std::stod(value)}, iterator);
                }
                else if (static_cast<char>(*iterator) == '"')
                {
                    const auto [stringValue, stringIterator] = parseString(iterator, end);
                    iterator = stringIterator;
                    return std::pair(Value{stringValue}, iterator);
                }
                else
                {
                    constexpr char trueString[] = {'t', 'r', 'u', 'e'};
                    constexpr char falseString[] = {'f', 'a', 'l', 's', 'e'};
                    constexpr char nullString[] = {'n', 'u', 'l', 'l'};

                    const auto [isTrue, trueIterator] = isSame(iterator, end,
                                                               std::begin(trueString),
                                                               std::end(trueString));
                    iterator = trueIterator;
                    if (isTrue)
                        return std::pair(Value{true}, iterator);

                    const auto [isFalse, falseIterator] = isSame(iterator, end,
                                                                 std::begin(falseString),
                                                                 std::end(falseString));
                    iterator = falseIterator;
                    if (isFalse)
                        return std::pair(Value{false}, iterator);

                    const auto [isNull, nullIterator] = isSame(iterator, end,
                                                               std::begin(nullString),
                                                               std::end(nullString));
                    iterator = nullIterator;
                    if (isNull)
                        return std::pair(Value{nullptr}, iterator);

                    throw ParseError{"Unexpected identifier"};
                }
            }

            static std::pair<std::string, Iterator> parseString(Iterator begin, Iterator end)
            {
                std::string result;
                Iterator iterator = begin;

                if (iterator == end || static_cast<char>(*iterator) != '"')
                    throw ParseError{"Invalid string"};

                ++iterator;

                while (iterator != end && static_cast<char>(*iterator) != '"')
                {
                    if (static_cast<char>(*iterator) == '\\')
                    {
                        if (++iterator == end)
                            throw ParseError{"Unterminated string literal"};

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
                                        throw ParseError{"Unexpected end of data"};

                                    std::uint8_t code = 0;

                                    if (static_cast<char>(*iterator) >= '0' && static_cast<char>(*iterator) <= '9')
                                        code = static_cast<std::uint8_t>(*iterator) - '0';
                                    else if (static_cast<char>(*iterator) >= 'a' && static_cast<char>(*iterator) <='f')
                                        code = static_cast<std::uint8_t>(*iterator) - 'a' + 10;
                                    else if (static_cast<char>(*iterator) >= 'A' && static_cast<char>(*iterator) <='F')
                                        code = static_cast<std::uint8_t>(*iterator) - 'A' + 10;
                                    else
                                        throw ParseError{"Invalid character code"};

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
                                throw ParseError{"Unrecognized escape character"};
                        }
                    }
                    else if (static_cast<std::uint8_t>(*iterator) <= 0x1F) // control char
                        throw ParseError{"Unterminated string literal"};
                    else
                        result.push_back(static_cast<char>(*iterator));

                    ++iterator;
                }

                if (iterator == end || static_cast<char>(*iterator) != '"')
                    throw ParseError{"Invalid string"};

                ++iterator;

                return std::pair(result, iterator);
            }
        };

        return Parser::parse(begin, end);
    }

    inline Value parse(const char* data)
    {
        auto end = data;
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
                if (byteOrderMark) result.assign(utf8ByteOrderMark.begin(),
                                                 utf8ByteOrderMark.end());
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

                        constexpr std::array<char, 16> digits{
                            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                            'a', 'b', 'c', 'd', 'e', 'f'
                        };
                        for (std::uint32_t p = 0; p < 4; ++p)
                            result.push_back(digits[(c >> (12 - p * 4)) & 0x0F]);
                    }
                    else
                        result.push_back(c);
            }

            static void encode(const Value& value, std::string& result, bool whitespaces, size_t level = 0)
            {
                const auto& v = value.getValue();

                if (std::holds_alternative<std::nullptr_t>(v))
                {
                    result.insert(result.end(), {'n', 'u', 'l', 'l'});
                }
                else if (auto d = std::get_if<double>(&v))
                {
                    const auto str = (std::floor(*d) == *d) ?
                        std::to_string(static_cast<std::int64_t>(*d)) :
                        std::to_string(*d);
                    result.insert(result.end(), str.begin(), str.end());
                }
                else if (auto s = std::get_if<Value::String>(&v))
                {
                    result.push_back('"');
                    encode(*s, result);
                    result.push_back('"');
                }
                else if (auto o = std::get_if<Value::Object>(&v))
                {
                    result.push_back('{');
                    if (whitespaces) result.push_back('\n');

                    for (auto entryIterator = o->begin(); entryIterator != o->end();)
                    {
                        const auto& [key, entryValue] = *entryIterator;
                        if (whitespaces) result.insert(result.end(), level + 1, '\t');
                        result.push_back('"');
                        encode(key, result);
                        result.insert(result.end(), {'"', ':'});
                        encode(entryValue, result, whitespaces, level + 1);
                        if (++entryIterator != o->end()) result.push_back(',');
                        if (whitespaces) result.push_back('\n');
                    }

                    if (whitespaces) result.insert(result.end(), level, '\t');
                    result.push_back('}');
                }
                else if (auto a = std::get_if<Value::Array>(&v))
                {
                    result.push_back('[');
                    if (whitespaces) result.push_back('\n');

                    for (auto entryIterator = a->begin(); entryIterator != a->end();)
                    {
                        const auto& entry = *entryIterator;
                        if (whitespaces) result.insert(result.end(), level + 1, '\t');
                        encode(entry, result, whitespaces, level + 1);
                        if (++entryIterator != a->end()) result.push_back(',');
                        if (whitespaces) result.push_back('\n');
                    }

                    if (whitespaces) result.insert(result.end(), level, '\t');
                    result.push_back(']');
                }
                else if (auto b = std::get_if<bool>(&v))
                {
                    if (*b) result.insert(result.end(), {'t', 'r', 'u', 'e'});
                    else result.insert(result.end(), {'f', 'a', 'l', 's', 'e'});
                }
                else
                    throw ParseError{"Unknown value type"};
            }
        };

        return Encoder::encode(value, whitespaces, byteOrderMark);
    }
}

#endif // OUZEL_FORMATS_JSON_HPP
