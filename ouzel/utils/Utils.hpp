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

    template <typename T>
    inline auto getVectorSize(const typename std::vector<T>& vec)
    {
        return sizeof(T) * vec.size();
    }

    template <class T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline T decodeBigEndian(const void* buffer)
    {
        auto bytes = static_cast<const uint8_t*>(buffer);
        T result = 0;

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            result |= static_cast<T>(bytes[sizeof(buffer) - i - 1] << (i * 8));

        return result;
    }

    template <class T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline T decodeLittleEndian(const void* buffer)
    {
        auto bytes = static_cast<const uint8_t*>(buffer);
        T result = 0;

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            result |= static_cast<T>(bytes[i] << (i * 8));

        return result;
    }

    template <class T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline void encodeBigEndian(void* buffer, const T value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            bytes[i] = static_cast<uint8_t>(value >> ((sizeof(T) - i - 1) * 8));
    }

    template <class T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline T encodeLittleEndian(void* buffer, const T value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            bytes[i] = static_cast<uint8_t>(value >> (i * 8));
    }

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    std::string hexToString(const T n, size_t len = 0)
    {
        static constexpr const char* digits = "0123456789ABCDEF";
        if (len == 0)
        {
            auto t = static_cast<uint64_t>(n);
            do
            {
                t >>= 4;
                ++len;
            }
            while (t);
        }

        std::string result(len, '0');
        for (size_t i = 0, j = (len - 1) * 4; i < len; ++i, j -= 4)
            result[i] = digits[(n >> j) & 0x0F];

        return result;
    }

    template <typename T>
    std::vector<T> explodeString(const T& str, char delimiter = ' ')
    {
        std::vector<T> result;
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
}

#endif // OUZEL_UTILS_UTILS_HPP
