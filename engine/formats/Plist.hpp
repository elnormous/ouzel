// Ouzel by Elviss Strazdins

#ifndef OUZEL_FORMATS_PLIST_HPP
#define OUZEL_FORMATS_PLIST_HPP

#include <map>
#include <string>
#include <vector>

namespace ouzel::plist
{
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
        enum class Type
        {
            dictionary,
            array,
            string,
            real,
            integer,
            boolean,
            data,
            date
        };

        using Dictionary = std::map<std::string, Value>;
        using Array = std::vector<Value>;
        using Data = std::vector<std::byte>;

        Value() = default;
        Value(const Dictionary& value):type{Type::dictionary}, dictionaryValue(value) {}
        Value(const Array& value):type{Type::array}, arrayValue(value) {}
        Value(bool value):type{Type::boolean}, booleanValue{value} {}
        template <typename T, typename std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
        Value(T value):type{Type::real}, realValue{static_cast<double>(value)} {}
        template <typename T, typename std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>>* = nullptr>
        Value(T value):type{Type::integer}, integerValue{static_cast<std::int64_t>(value)} {}
        Value(const std::string& value):type{Type::string}, stringValue{value} {}
        Value(const char* value):type{Type::string}, stringValue{value} {}
        Value(const Data& value):type{Type::data}, dataValue{value} {}

        Value& operator=(const Dictionary& value)
        {
            type = Type::dictionary;
            dictionaryValue = value;
            return *this;
        }

        Value& operator=(const Array& value)
        {
            type = Type::array;
            arrayValue = value;
            return *this;
        }

        Value& operator=(bool value)
        {
            type = Type::boolean;
            booleanValue = value;
            return *this;
        }

        template <typename T, typename std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
        Value& operator=(T value)
        {
            type = Type::real;
            realValue = static_cast<double>(value);
            return *this;
        }

        template <typename T, typename std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>>* = nullptr>
        Value& operator=(T value)
        {
            type = Type::integer;
            integerValue = static_cast<std::int64_t>(value);
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

        Value& operator=(const Data& value)
        {
            type = Type::data;
            dataValue = value;
            return *this;
        }

        Type getType() const noexcept { return type; }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, std::string>>* = nullptr>
        T& as() noexcept
        {
            type = Type::string;
            return stringValue;
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, std::string>>* = nullptr>
        const T& as() const
        {
            if (type != Type::string) throw TypeError("Wrong type");
            return stringValue;
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, bool>>* = nullptr>
        T as() const
        {
            if (type != Type::boolean) throw TypeError("Wrong type");
            return booleanValue;
        }

        template <typename T, typename std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
        T as() const
        {
            if (type != Type::real) throw TypeError("Wrong type");
            return static_cast<T>(realValue);
        }

        template <typename T, typename std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>>* = nullptr>
        T as() const
        {
            if (type != Type::integer) throw TypeError("Wrong type");
            return static_cast<T>(integerValue);
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Dictionary>>* = nullptr>
        T& as() noexcept
        {
            type = Type::dictionary;
            return dictionaryValue;
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Dictionary>>* = nullptr>
        const T& as() const
        {
            if (type != Type::dictionary) throw TypeError("Wrong type");
            return dictionaryValue;
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Array>>* = nullptr>
        T& as() noexcept
        {
            type = Type::array;
            return arrayValue;
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Array>>* = nullptr>
        const T& as() const
        {
            if (type != Type::array) throw TypeError("Wrong type");
            return arrayValue;
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Data>>* = nullptr>
        const T& as() const
        {
            if (type != Type::data) throw TypeError("Wrong type");
            return dataValue;
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

        auto hasMember(const std::string& member) const
        {
            if (type != Type::dictionary) throw TypeError("Wrong type");
            return dictionaryValue.find(member) != dictionaryValue.end();
        }

        Value& operator[](const std::string& member)
        {
            type = Type::dictionary;
            return dictionaryValue[member];
        }

        const Value& operator[](const std::string& member) const
        {
            if (type != Type::dictionary) throw TypeError("Wrong type");

            const auto i = dictionaryValue.find(member);
            if (i != dictionaryValue.end())
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

        auto getSize() const
        {
            if (type != Type::array) throw TypeError("Wrong type");
            return arrayValue.size();
        }

        void resize(std::size_t size)
        {
            if (type != Type::array) throw TypeError("Wrong type");
            arrayValue.resize(size);
        }

        void pushBack(std::byte value)
        {
            if (type != Type::data) throw TypeError("Wrong type");
            dataValue.push_back(value);
        }

        void pushBack(const Value& value)
        {
            if (type != Type::array) throw TypeError("Wrong type");
            arrayValue.push_back(value);
        }

    private:
        Type type = Type::dictionary;
        Dictionary dictionaryValue;
        Array arrayValue;
        std::string stringValue;
        double realValue = 0.0;
        std::int64_t integerValue = 0;
        bool booleanValue = false;
        Data dataValue;
    };

    enum class Format
    {
        ascii,
        xml
    };

    inline std::string encode(const Value& value, Format format, bool whitespaces = false)
    {
        class AsciiEncoder final
        {
        public:
            static std::string encode(const Value& value, bool whitespaces)
            {
                std::string result = "// !$*UTF8*$!\n";
                encode(value, result, whitespaces);
                return result;
            }

        private:
            static void encode(const std::string& s, std::string& result)
            {
                if (!s.empty())
                {
                    bool hasSpecialChars = false;
                    for (const auto c : s)
                        if ((c < 'a' || c > 'z') &&
                            (c < 'A' || c > 'Z') &&
                            (c < '0' || c > '9') &&
                            c != '_' && c != '$' && c != '/' &&
                            c != ':' && c != '.' && c != '-')
                        {
                            hasSpecialChars = true;
                            break;
                        }

                    if (hasSpecialChars) result.push_back('"');
                    for (const auto c : s)
                    {
                        if (c == '"' || c == '\\') result += '\\';
                        result += c;
                    }
                    if (hasSpecialChars) result.push_back('"');
                }
                else
                    result += "\"\"";
            }

            static void encode(const Value& value, std::string& result, bool whitespaces, size_t level = 0)
            {
                switch (value.getType())
                {
                    case Value::Type::dictionary:
                    {
                        result.push_back('{');
                        if (whitespaces) result.push_back('\n');
                        for (const auto& entry : value.as<Value::Dictionary>())
                        {
                            if (whitespaces) result.insert(result.end(), level + 1, '\t');
                            encode(entry.first, result);
                            if (whitespaces) result.push_back(' ');
                            result.push_back('=');
                            if (whitespaces) result.push_back(' ');
                            encode(entry.second, result, whitespaces, level + 1);
                            result.push_back(';');
                            if (whitespaces) result.push_back('\n');
                        }
                        if (whitespaces) result.insert(result.end(), level, '\t');
                        result += "}";
                        break;
                    }
                    case Value::Type::array:
                    {
                        result.push_back('(');
                        if (whitespaces) result.push_back('\n');
                        for (const auto& child : value.as<Value::Array>())
                        {
                            if (whitespaces) result.insert(result.end(), level + 1, '\t');
                            encode(child, result, whitespaces, level + 1);
                            result.push_back(',');
                            if (whitespaces) result.push_back('\n');
                        }
                        if (whitespaces) result.insert(result.end(), level, '\t');
                        result += ')';
                        break;
                    }
                    case Value::Type::string:
                        encode(value.as<std::string>(), result);
                        break;
                    case Value::Type::real:
                        result += std::to_string(value.as<double>());
                        break;
                    case Value::Type::integer:
                        result += std::to_string(value.as<std::int64_t>());
                        break;
                    case Value::Type::boolean:
                        result += value.as<bool>() ? "YES" : "NO";
                        break;
                    case Value::Type::data:
                        result += '<';
                        for (const auto b : value.as<Value::Data>())
                        {
                            constexpr char digits[] = "0123456789ABCDEF";
                            result += digits[(static_cast<std::size_t>(b) >> 4) & 0x0F];
                            result += digits[static_cast<std::size_t>(b) & 0x0F];
                        }
                        result += '>';
                        break;
                    case Value::Type::date:
                        throw std::runtime_error("Date fields are not supported");
                };
            }
        };

        class XmlEncoder final
        {
        public:
            static std::string encode(const Value& value, bool whitespaces)
            {
                std::string result = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
                if (whitespaces) result.push_back('\n');
                result += "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">";
                if (whitespaces) result.push_back('\n');
                result += "<plist version=\"1.0\">";
                if (whitespaces) result.push_back('\n');
                encode(value, result, whitespaces);
                if (whitespaces) result.push_back('\n');
                result += "</plist>";
                if (whitespaces) result.push_back('\n');
                return result;
            }

        private:
            static void encode(const std::string& s, std::string& result)
            {
                for (const auto c : s)
                    if (c == '<') result += "&lt;";
                    else if (c == '>') result += "&gt;";
                    else if (c == '&') result += "&amp;";
                    else result.push_back(c);
            }

            static void encode(const Value& value, std::string& result, bool whitespaces, size_t level = 0)
            {
                switch (value.getType())
                {
                    case Value::Type::dictionary:
                    {
                        result += "<dict>";
                        if (whitespaces) result.push_back('\n');
                        for (const auto& entry : value.as<Value::Dictionary>())
                        {
                            if (whitespaces) result.insert(result.end(), level + 1, '\t');
                            result += "<key>";
                            encode(entry.first, result);
                            result += "</key>";
                            if (whitespaces) result += '\n';
                            if (whitespaces) result.insert(result.end(), level + 1, '\t');
                            encode(entry.second, result, whitespaces, level + 1);
                            if (whitespaces) result += '\n';
                        }
                        result.insert(result.end(), level, '\t');
                        result += "</dict>";
                        break;
                    }
                    case Value::Type::array:
                    {
                        result += "<array>";
                        if (whitespaces) result.push_back('\n');
                        for (const auto& child : value.as<Value::Array>())
                        {
                            if (whitespaces) result.insert(result.end(), level + 1, '\t');
                            encode(child, result, whitespaces, level + 1);
                            if (whitespaces) result.push_back('\n');
                        }
                        if (whitespaces) result.insert(result.end(), level, '\t');
                        result += "</array>";
                        break;
                    }
                    case Value::Type::string:
                        result += "<string>";
                        encode(value.as<std::string>(), result);
                        result += "</string>";
                        break;
                    case Value::Type::real:
                        result += "<real>";
                        result += std::to_string(value.as<double>());
                        result += "</real>";
                        break;
                    case Value::Type::integer:
                        result += "<integer>";
                        result += std::to_string(value.as<std::int64_t>());
                        result += "</integer>";
                        break;
                    case Value::Type::boolean:
                        result += value.as<bool>() ? "<true/>" : "<false/>";
                        break;
                    case Value::Type::data:
                    {
                        result += "<data>";
                        constexpr char chars[] = {
                            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                            'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                            'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
                        };
                        std::size_t c = 0;
                        std::uint8_t charArray[3];
                        for (const auto b : value.as<Value::Data>())
                        {
                            charArray[c++] = static_cast<std::uint8_t>(b);
                            if (c == 3)
                            {
                                result += chars[static_cast<std::uint8_t>((charArray[0] & 0xFC) >> 2)];
                                result += chars[static_cast<std::uint8_t>(((charArray[0] & 0x03) << 4) + ((charArray[1] & 0xF0) >> 4))];
                                result += chars[static_cast<std::uint8_t>(((charArray[1] & 0x0F) << 2) + ((charArray[2] & 0xC0) >> 6))];
                                result += chars[static_cast<std::uint8_t>(charArray[2] & 0x3f)];
                                c = 0;
                            }
                        }
                        if (c)
                        {
                            result += chars[static_cast<std::uint8_t>((charArray[0] & 0xFC) >> 2)];

                            if (c == 1)
                                result += chars[static_cast<std::uint8_t>((charArray[0] & 0x03) << 4)];
                            else
                            {
                                result += chars[static_cast<std::uint8_t>(((charArray[0] & 0x03) << 4) + ((charArray[1] & 0xF0) >> 4))];

                                if (c == 2)
                                    result += chars[static_cast<std::uint8_t>((charArray[1] & 0x0F) << 2)];
                                else if (c == 3)
                                    result += chars[static_cast<std::uint8_t>(((charArray[1] & 0x0F) << 2) + ((charArray[2] & 0xC0) >> 6))];
                            }

                            while (++c < 4) result += '=';
                        }
                        result += "</data>";
                        break;
                    }
                    case Value::Type::date:
                        throw std::runtime_error("Date fields are not supported");
                };
            }
        };

        switch (format)
        {
            case Format::ascii: return AsciiEncoder::encode(value, whitespaces);
            case Format::xml: return XmlEncoder::encode(value, whitespaces);
        }

        throw std::runtime_error("Unsupported format");
    }
}

#endif // OUZEL_FORMATS_PLIST_HPP
