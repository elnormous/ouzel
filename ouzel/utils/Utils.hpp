// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_UTILS_UTILS_HPP
#define OUZEL_UTILS_UTILS_HPP

#include <cstdint>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

namespace ouzel
{
    extern std::mt19937 randomEngine;

    template <class T>
    inline auto getVectorSize(const T& vec) noexcept
    {
        return sizeof(typename T::value_type) * vec.size();
    }

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline auto decodeBigEndian(const uint8_t* buffer) noexcept
    {
        T result = 0;

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            result |= static_cast<T>(buffer[sizeof(buffer) - i - 1] << (i * 8));

        return result;
    }

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline auto decodeLittleEndian(const uint8_t* buffer) noexcept
    {
        T result = 0;

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            result |= static_cast<T>(buffer[i] << (i * 8));

        return result;
    }

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline void encodeBigEndian(uint8_t* buffer, const T value) noexcept
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            bytes[i] = static_cast<uint8_t>(value >> ((sizeof(T) - i - 1) * 8));
    }

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline void encodeLittleEndian(uint8_t* buffer, const T value) noexcept
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            bytes[i] = static_cast<uint8_t>(value >> (i * 8));
    }

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    auto hexToString(const T n, const size_t len = 0)
    {
        constexpr char digits[] = "0123456789abcdef";

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
    constexpr T toLower(T c) noexcept
    {
        return (c >= 'A' && c <= 'Z') ? c - T('A' - 'a') : c;
    }
}

#endif // OUZEL_UTILS_UTILS_HPP
