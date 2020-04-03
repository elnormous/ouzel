// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_UTILS_PLIST_HPP
#define OUZEL_UTILS_PLIST_HPP

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
                Number,
                Data
            };

            using Dictionary = std::map<std::string, Value>;
            using Array = std::vector<Value>;
            using Data = std::vector<uint8_t>;

            Value() = default;
            Value(const Dictionary& value):type{Type::Dictionary}, dictionaryValue{value} {}
            Value(const Array& value):type{Type::Array}, arrayValue{value} {}
            template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
            Value(T value):type{Type::Number}, numberValue{static_cast<double>(value)} {}
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

            template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
            Value& operator=(T value)
            {
                type = Type::Number;
                numberValue = static_cast<double>(value);
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

            template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
            inline T as() const
            {
                if (type != Type::Number)
                    throw TypeError("Wrong type");

                return static_cast<T>(numberValue);
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
            double numberValue;
            Data dataValue;
        };

        inline std::string encode(const Value& value)
        {
            struct Encoder final
            {
                static std::string& encode(const std::string& s, std::string& result)
                {
                    if (!s.empty())
                    {
                        bool hasSpecialChars = false;
                        for (auto c : s)
                            if ((c < 'a' || c > 'z') &&
                                (c < 'A' || c > 'Z') &&
                                (c < '0' || c > '9'))
                            {
                                hasSpecialChars = true;
                                break;
                            }

                        if (hasSpecialChars) result.push_back('"');
                        for (const auto c : s)
                        {
                            if (c == '"') result += '\\';
                            result += c;
                        }
                        if (hasSpecialChars) result.push_back('"');
                    }
                    else
                        result += "\"\"";

                    return result;
                }

                static std::string& encode(const Value& value, std::string& result, size_t level = 0)
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
                            result += ")";
                            break;
                        }
                        case Value::Type::String:
                            encode(value.as<std::string>(), result);
                            break;
                        case Value::Type::Number:
                        {
                            double number = value.as<double>();
                            double intPart;
                            if (std::modf(number, &intPart) != 0.0)
                                result += std::to_string(number);
                            else
                                result += std::to_string(static_cast<int64_t>(intPart));
                            break;
                        }
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
                    };

                    return result;
                }
            };

            std::string result = "// !$*UTF8*$!\n";
            return Encoder::encode(value, result);
        }
    }
}

#endif // OUZEL_UTILS_PLIST_HPP
