// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_UTILS_OBF_HPP
#define OUZEL_UTILS_OBF_HPP

#include <cassert>
#include <cstdint>
#include <limits>
#include <map>
#include <string>
#include <vector>

namespace ouzel
{
    namespace obf
    {
        class Value final
        {
        public:
            using ByteArray = std::vector<uint8_t>;
            using Object = std::map<uint32_t, Value>;
            using Array = std::vector<Value>;
            using Dictionary = std::map<std::string, Value>;

            enum class Marker: uint8_t
            {
                Int8 = 1,
                Int16 = 2,
                Int32 = 3,
                Int64 = 4,
                Float = 5,
                Double = 6,
                String = 7,
                LongString = 8,
                ByteArray = 9,
                Object = 10,
                Array = 11,
                Dictionary = 12
            };

            enum class Type
            {
                Int,
                Float,
                Double,
                String,
                ByteArray,
                Object,
                Array,
                Dictionary
            };

            Value() = default;
            Value(Type initType): type(initType) {}
            Value(uint8_t value):
                type(Type::Int), intValue(value)
            {
            }
            Value(uint16_t value):
                type(Type::Int), intValue(value)
            {
            }
            Value(uint32_t value):
                type(Type::Int), intValue(value)
            {
            }
            Value(uint64_t value):
                type(Type::Int), intValue(value)
            {
            }
            Value(float value): type(Type::Float), doubleValue(value) {}
            Value(double value): type(Type::Double), doubleValue(value) {}
            Value(const std::string& value):
                type(Type::String), stringValue(value)
            {
            }
            Value(const ByteArray& value): type(Type::ByteArray), byteArrayValue(value) {}
            Value(const Object& value): type(Type::Object), objectValue(value) {}
            Value(const Array& value): type(Type::Array), arrayValue(value) {}
            Value(const Dictionary& value): type(Type::Dictionary), dictionaryValue(value) {}

            inline Value& operator=(Type newType)
            {
                type = newType;

                switch (type)
                {
                    case Type::Int:
                        intValue = 0;
                        break;
                    case Type::Float:
                    case Type::Double:
                        doubleValue = 0.0;
                        break;
                    case Type::String:
                        stringValue.clear();
                        break;
                    case Type::ByteArray:
                        byteArrayValue.clear();
                        break;
                    case Type::Object:
                        objectValue.clear();
                        break;
                    case Type::Array:
                        arrayValue.clear();
                        break;
                    case Type::Dictionary:
                        dictionaryValue.clear();
                        break;
                    default:
                        break;
                }

                return *this;
            }

            inline Value& operator=(uint8_t value)
            {
                type = Type::Int;
                intValue = value;

                return *this;
            }

            inline Value& operator=(uint16_t value)
            {
                type = Type::Int;
                intValue = value;

                return *this;
            }

            inline Value& operator=(uint32_t value)
            {
                type = Type::Int;
                intValue = value;

                return *this;
            }

            inline Value& operator=(uint64_t value)
            {
                type = Type::Int;
                intValue = value;

                return *this;
            }

            inline Value& operator=(float value)
            {
                type = Type::Float;
                doubleValue = value;

                return *this;
            }

            inline Value& operator=(double value)
            {
                type = Type::Double;
                doubleValue = value;

                return *this;
            }

            inline Value& operator=(const std::string& value)
            {
                type = Type::String;
                stringValue = value;

                return *this;
            }

            inline Value& operator=(const ByteArray& value)
            {
                type = Type::ByteArray;
                byteArrayValue = value;

                return *this;
            }

            inline Value& operator=(const Object& value)
            {
                type = Type::Object;
                objectValue = value;

                return *this;
            }

            inline Value& operator=(const Array& value)
            {
                type = Type::Array;
                arrayValue = value;

                return *this;
            }

            inline Value& operator=(const Dictionary& value)
            {
                type = Type::Dictionary;
                dictionaryValue = value;

                return *this;
            }

            inline auto getType() const noexcept { return type; }
            inline auto isIntType() const noexcept { return type == Type::Int; }
            inline auto isFloatType() const noexcept { return type == Type::Float || type == Type::Double; }
            inline auto isStringType() const noexcept { return type == Type::String; }

            uint32_t decode(const std::vector<uint8_t>& buffer, uint32_t offset = 0);
            uint32_t encode(std::vector<uint8_t>& buffer) const;

            inline bool operator!()
            {
                switch (type)
                {
                    case Type::Int:
                        return intValue == 0;
                    case Type::Float:
                    case Type::Double:
                        return doubleValue == 0.0;
                    case Type::String:
                    case Type::ByteArray:
                    case Type::Object:
                    case Type::Array:
                    case Type::Dictionary:
                        return false;
                    default:
                        return true;
                }
            }

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

            template <typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
            T as() const
            {
                assert(type == Type::Int);
                return static_cast<T>(intValue);
            }

            template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
            T as() const
            {
                assert(type == Type::Float || type == Type::Double);
                return static_cast<T>(doubleValue);
            }

            template <typename T, typename std::enable_if<std::is_same<T, ByteArray>::value>::type* = nullptr>
            inline ByteArray& as()
            {
                type = Type::ByteArray;
                return byteArrayValue;
            }

            template <typename T, typename std::enable_if<std::is_same<T, ByteArray>::value>::type* = nullptr>
            inline const ByteArray& as() const
            {
                assert(type == Type::ByteArray);
                return byteArrayValue;
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

            template <typename T, typename std::enable_if<std::is_same<T, Dictionary>::value>::type* = nullptr>
            inline Dictionary& as()
            {
                type = Type::Dictionary;
                return dictionaryValue;
            }

            template <typename T, typename std::enable_if<std::is_same<T, Dictionary>::value>::type* = nullptr>
            inline const Dictionary& as() const
            {
                assert(type == Type::Dictionary);
                return dictionaryValue;
            }

            inline Array::iterator begin()
            {
                assert(type == Type::Array);
                return arrayValue.begin();
            }

            inline Array::const_iterator begin() const
            {
                assert(type == Type::Array);
                return arrayValue.begin();
            }

            inline Array::iterator end()
            {
                assert(type == Type::Array);
                return arrayValue.end();
            }

            inline Array::const_iterator end() const
            {
                assert(type == Type::Array);
                return arrayValue.end();
            }

            inline auto getSize() const noexcept
            {
                assert(type == Type::Array);
                return static_cast<uint32_t>(arrayValue.size());
            }

            inline Value operator[](uint32_t key) const
            {
                assert(type == Type::Object || type == Type::Array);

                if (type == Type::Object)
                {
                    auto i = objectValue.find(key);

                    if (i != objectValue.end())
                        return i->second;
                }
                else if (type == Type::Array)
                {
                    if (key < arrayValue.size())
                        return arrayValue[key];
                }

                return Value();
            }

            inline Value& operator[](uint32_t key)
            {
                assert(type == Type::Object || type == Type::Array);

                if (type == Type::Object)
                    return objectValue[key];
                else
                {
                    type = Type::Array;
                    return arrayValue[key];
                }
            }

            inline Value operator[](const std::string& key) const
            {
                assert(type == Type::Dictionary);

                auto i = dictionaryValue.find(key);

                if (i != dictionaryValue.end())
                    return i->second;

                return Value();
            }

            inline Value& operator[](const std::string& key)
            {
                assert(type == Type::Dictionary);

                return dictionaryValue[key];
            }

            inline bool hasElement(uint32_t key) const
            {
                assert(type == Type::Object || type == Type::Array);

                if (type == Type::Object)
                    return objectValue.find(key) != objectValue.end();
                else if (type == Type::Array)
                    return key < arrayValue.size();
                else
                    return false;
            }

            inline bool hasElement(const std::string& key) const
            {
                assert(type == Type::Dictionary);

                return dictionaryValue.find(key) != dictionaryValue.end();
            }

            inline void append(const Value& node)
            {
                arrayValue.push_back(node);
            }

        private:
            Type type = Type::Object;
            union
            {
                uint64_t intValue = 0;
                double doubleValue;
            };
            std::string stringValue;
            ByteArray byteArrayValue;
            Object objectValue;
            Array arrayValue;
            Dictionary dictionaryValue;
        };
    } // namespace obf
} // namespace ouzel

#endif // OUZEL_UTILS_OBF_HPP
