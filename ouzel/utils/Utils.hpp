// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_UTILS_UTILS_HPP
#define OUZEL_UTILS_UTILS_HPP

#include <cstdint>
#include <random>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

namespace ouzel
{
    extern std::mt19937 randomEngine;

    template <class T>
    inline auto getVectorSize(const T& vec)
    {
        return sizeof(typename T::value_type) * vec.size();
    }

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline auto decodeBigEndian(const void* buffer)
    {
        auto bytes = static_cast<const uint8_t*>(buffer);
        T result = 0;

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            result |= static_cast<T>(bytes[sizeof(buffer) - i - 1] << (i * 8));

        return result;
    }

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline auto decodeLittleEndian(const void* buffer)
    {
        auto bytes = static_cast<const uint8_t*>(buffer);
        T result = 0;

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            result |= static_cast<T>(bytes[i] << (i * 8));

        return result;
    }

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline void encodeBigEndian(void* buffer, const T value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            bytes[i] = static_cast<uint8_t>(value >> ((sizeof(T) - i - 1) * 8));
    }

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline void encodeLittleEndian(void* buffer, const T value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            bytes[i] = static_cast<uint8_t>(value >> (i * 8));
    }

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    auto hexToString(const T n, const size_t len = 0)
    {
        static constexpr const char* digits = "0123456789ABCDEF";

        const size_t count = (len == 0) ? n / 16 + 1 : len;

        std::string result(count, '0');
        for (size_t i = 0, j = (count - 1) * 4; i < count; ++i, j -= 4)
            result[i] = digits[(n >> j) & 0x0F];

        return result;
    }

    inline auto explodeString(const std::string& str, char delimiter = ' ')
    {
        std::vector<std::string> result;
        size_t pos;
        size_t initialPos = 0;
        while ((pos = str.find(delimiter, initialPos)) != std::string::npos)
        {
            if (pos != initialPos)
                result.push_back(str.substr(initialPos, pos - initialPos));
            initialPos = pos + 1;
        }
        if (initialPos < str.size())
            result.push_back(str.substr(initialPos, str.size() - initialPos + 1));
        return result;
    }

    template <class T>
    constexpr T toLower(T c)
    {
        return (c >= 'A' && c <= 'Z') ? c - ('A' - 'a') : c;
    }
}

#endif // OUZEL_UTILS_UTILS_HPP
