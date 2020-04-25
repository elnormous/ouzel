// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_UTILS_PLIST_HPP
#define OUZEL_UTILS_PLIST_HPP

#include <map>
#include <string>
#include <vector>

namespace ouzel
{
    namespace plist
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
                Dictionary,
                Array,
                String,
                Real,
                Integer,
                Boolean,
                Data,
                Date
            };

            using Dictionary = std::map<std::string, Value>;
            using Array = std::vector<Value>;
            using Data = std::vector<uint8_t>;

            Value() = default;
            Value(const Dictionary& value):type{Type::Dictionary}, dictionaryValue(value) {}
            Value(const Array& value):type{Type::Array}, arrayValue(value) {}
            Value(bool value):type{Type::Boolean}, booleanValue{value} {}
            template <class T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
            Value(T value):type{Type::Real}, realValue{static_cast<double>(value)} {}
            template <class T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
            Value(T value):type{Type::Integer}, integerValue{static_cast<int64_t>(value)} {}
            Value(const std::string& value):type{Type::String}, stringValue{value} {}
            Value(const char* value):type{Type::String}, stringValue{value} {}
            Value(const Data& value):type{Type::Data}, dataValue{value} {}

            Value& operator=(const Dictionary& value)
            {
                type = Type::Dictionary;
                dictionaryValue = value;
                return *this;
            }

            Value& operator=(const Array& value)
            {
                type = Type::Array;
                arrayValue = value;
                return *this;
            }

            Value& operator=(bool value)
            {
                type = Type::Boolean;
                booleanValue = value;
                return *this;
            }

            template <class T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
            Value& operator=(T value)
            {
                type = Type::Real;
                realValue = static_cast<double>(value);
                return *this;
            }

            template <class T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
            Value& operator=(T value)
            {
                type = Type::Integer;
                integerValue = static_cast<int64_t>(value);
                return *this;
            }

            Value& operator=(const std::string& value)
            {
                type = Type::String;
                stringValue = value;
                return *this;
            }

            Value& operator=(const char* value)
            {
                type = Type::String;
                stringValue = value;
                return *this;
            }

            Value& operator=(const Data& value)
            {
                type = Type::Data;
                dataValue = value;
                return *this;
            }

            Type getType() const noexcept { return type; }

            template <typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
            inline T& as() noexcept
            {
                type = Type::String;
                return stringValue;
            }

            template <typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
            inline const T& as() const
            {
                if (type != Type::String) throw TypeError("Wrong type");
                return stringValue;
            }

            template <typename T, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
            inline T as() const
            {
                if (type != Type::Boolean) throw TypeError("Wrong type");
                return booleanValue;
            }

            template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
            inline T as() const
            {
                if (type != Type::Real) throw TypeError("Wrong type");
                return static_cast<T>(realValue);
            }

            template <class T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
            inline T as() const
            {
                if (type != Type::Integer) throw TypeError("Wrong type");
                return static_cast<T>(integerValue);
            }

            template <typename T, typename std::enable_if<std::is_same<T, Dictionary>::value>::type* = nullptr>
            inline T& as() noexcept
            {
                type = Type::Dictionary;
                return dictionaryValue;
            }

            template <typename T, typename std::enable_if<std::is_same<T, Dictionary>::value>::type* = nullptr>
            inline const T& as() const
            {
                if (type != Type::Dictionary) throw TypeError("Wrong type");
                return dictionaryValue;
            }

            template <typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
            inline T& as() noexcept
            {
                type = Type::Array;
                return arrayValue;
            }

            template <typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
            inline const T& as() const
            {
                if (type != Type::Array) throw TypeError("Wrong type");
                return arrayValue;
            }

            template <typename T, typename std::enable_if<std::is_same<T, Data>::value>::type* = nullptr>
            inline const T& as() const
            {
                if (type != Type::Data) throw TypeError("Wrong type");
                return dataValue;
            }

            Array::iterator begin()
            {
                if (type != Type::Array) throw TypeError("Wrong type");
                return arrayValue.begin();
            }

            Array::iterator end()
            {
                if (type != Type::Array) throw TypeError("Wrong type");
                return arrayValue.end();
            }

            Array::const_iterator begin() const
            {
                if (type != Type::Array) throw TypeError("Wrong type");
                return arrayValue.begin();
            }

            Array::const_iterator end() const
            {
                if (type != Type::Array) throw TypeError("Wrong type");
                return arrayValue.end();
            }

            inline auto hasMember(const std::string& member) const
            {
                if (type != Type::Dictionary) throw TypeError("Wrong type");
                return dictionaryValue.find(member) != dictionaryValue.end();
            }

            inline Value& operator[](const std::string& member)
            {
                type = Type::Dictionary;
                return dictionaryValue[member];
            }

            inline const Value& operator[](const std::string& member) const
            {
                if (type != Type::Dictionary) throw TypeError("Wrong type");

                auto i = dictionaryValue.find(member);
                if (i != dictionaryValue.end())
                    return i->second;
                else
                    throw RangeError("Member does not exist");
            }

            inline Value& operator[](std::size_t index)
            {
                type = Type::Array;
                if (index >= arrayValue.size()) arrayValue.resize(index + 1);
                return arrayValue[index];
            }

            inline const Value& operator[](std::size_t index) const
            {
                if (type != Type::Array) throw TypeError("Wrong type");

                if (index < arrayValue.size())
                    return arrayValue[index];
                else
                    throw RangeError("Index out of range");
            }

            inline auto getSize() const
            {
                if (type != Type::Array) throw TypeError("Wrong type");
                return arrayValue.size();
            }

            inline void resize(std::size_t size)
            {
                if (type != Type::Array) throw TypeError("Wrong type");
                arrayValue.resize(size);
            }

            inline void pushBack(uint8_t value)
            {
                if (type != Type::Data) throw TypeError("Wrong type");
                dataValue.push_back(value);
            }

            inline void pushBack(const Value& value)
            {
                if (type != Type::Array) throw TypeError("Wrong type");
                arrayValue.push_back(value);
            }

        private:
            Type type = Type::Dictionary;
            Dictionary dictionaryValue;
            Array arrayValue;
            std::string stringValue;
            double realValue = 0.0;
            int64_t integerValue = 0;
            bool booleanValue = false;
            Data dataValue;
        };

        enum class Format
        {
            Ascii,
            Xml
        };

        inline std::string encode(const Value& value, Format format)
        {
            class AsciiEncoder final
            {
            public:
                static std::string encode(const Value& value)
                {
                    std::string result = "// !$*UTF8*$!\n";
                    encode(value, result);
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

                static void encode(const Value& value, std::string& result, size_t level = 0)
                {
                    switch (value.getType())
                    {
                        case Value::Type::Dictionary:
                        {
                            result += "{\n";
                            for (const auto& entry : value.as<Value::Dictionary>())
                            {
                                result.insert(result.end(), level + 1, '\t');
                                encode(entry.first, result);
                                result += " = ";
                                encode(entry.second, result, level + 1);
                                result += ";\n";
                            }
                            result.insert(result.end(), level, '\t');
                            result += "}";
                            break;
                        }
                        case Value::Type::Array:
                        {
                            result += "(\n";
                            for (const auto& child : value.as<Value::Array>())
                            {
                                result.insert(result.end(), level + 1, '\t');
                                encode(child, result, level + 1);
                                result += ",\n";
                            }
                            result.insert(result.end(), level, '\t');
                            result += ')';
                            break;
                        }
                        case Value::Type::String:
                            encode(value.as<std::string>(), result);
                            break;
                        case Value::Type::Real:
                            result += std::to_string(value.as<double>());
                            break;
                        case Value::Type::Integer:
                            result += std::to_string(value.as<int64_t>());
                            break;
                        case Value::Type::Boolean:
                            result += value.as<bool>() ? "YES" : "NO";
                            break;
                        case Value::Type::Data:
                            result += '<';
                            for (const auto b : value.as<Value::Data>())
                            {
                                constexpr char digits[] = "0123456789ABCDEF";
                                result += digits[(b >> 4) & 0x0F];
                                result += digits[(b >> 0) & 0x0F];
                            }
                            result += '>';
                            break;
                        case Value::Type::Date:
                            throw std::runtime_error("Date fields are not supported");
                    };
                }
            };

            class XmlEncoder final
            {
            public:
                static std::string encode(const Value& value)
                {
                    std::string result = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                        "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
                        "<plist version=\"1.0\">\n";
                    encode(value, result);
                    result += "\n</plist>\n";
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

                static void encode(const Value& value, std::string& result, size_t level = 0)
                {
                    switch (value.getType())
                    {
                        case Value::Type::Dictionary:
                        {
                            result += "<dict>\n";
                            for (const auto& entry : value.as<Value::Dictionary>())
                            {
                                result.insert(result.end(), level + 1, '\t');
                                result += "<key>";
                                encode(entry.first, result);
                                result += "</key>\n";
                                result.insert(result.end(), level + 1, '\t');
                                encode(entry.second, result, level + 1);
                                result += '\n';
                            }
                            result.insert(result.end(), level, '\t');
                            result += "</dict>";
                            break;
                        }
                        case Value::Type::Array:
                        {
                            result += "<array>\n";
                            for (const auto& child : value.as<Value::Array>())
                            {
                                result.insert(result.end(), level + 1, '\t');
                                encode(child, result, level + 1);
                                result += '\n';
                            }
                            result.insert(result.end(), level, '\t');
                            result += "</array>";
                            break;
                        }
                        case Value::Type::String:
                            result += "<string>";
                            encode(value.as<std::string>(), result);
                            result += "</string>";
                            break;
                        case Value::Type::Real:
                            result += "<real>";
                            result += std::to_string(value.as<double>());
                            result += "</real>";
                            break;
                        case Value::Type::Integer:
                            result += "<integer>";
                            result += std::to_string(value.as<int64_t>());
                            result += "</integer>";
                            break;
                        case Value::Type::Boolean:
                            result += value.as<bool>() ? "<true/>" : "<false/>";
                            break;
                        case Value::Type::Data:
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
                                charArray[c++] = b;
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
                        case Value::Type::Date:
                            throw std::runtime_error("Date fields are not supported");
                    };
                }
            };

            switch (format)
            {
                case Format::Ascii: return AsciiEncoder::encode(value);
                case Format::Xml: return XmlEncoder::encode(value);
            }

            throw std::runtime_error("Unsupported format");
        }
    }
}

#endif // OUZEL_UTILS_PLIST_HPP
