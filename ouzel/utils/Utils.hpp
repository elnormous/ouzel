// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_UTILS_UTILS_HPP
#define OUZEL_UTILS_UTILS_HPP

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <limits>
#include <random>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

namespace ouzel
{
    extern std::mt19937 randomEngine;

    template<typename T>
    inline size_t getVectorSize(const typename std::vector<T>& vec)
    {
        return sizeof(T) * vec.size();
    }

    template<class T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline T decodeBigEndian(const void* buffer)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(buffer);
        T result = 0;

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            result |= static_cast<T>(bytes[sizeof(buffer) - i - 1] << (i * 8));

        return result;
    }

    template<class T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline T decodeLittleEndian(const void* buffer)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(buffer);
        T result = 0;

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            result |= static_cast<T>(bytes[i] << (i * 8));

        return result;
    }

    template<class T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline void encodeBigEndian(void* buffer, T value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            bytes[i] = static_cast<uint8_t>(value >> ((sizeof(T) - i - 1) * 8));
    }

    template<class T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline T encodeLittleEndian(void* buffer, T value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        for (uintptr_t i = 0; i < sizeof(T); ++i)
            bytes[i] = static_cast<uint8_t>(value >> (i * 8));
    }

    template<typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    std::string hexToString(T n, size_t len = 0)
    {
        static constexpr const char* digits = "0123456789ABCDEF";
        if (len == 0)
        {
            len = 1;
            while (static_cast<size_t>(1ull << (len * 4)) <= static_cast<size_t>(n)) ++len;
        }

        std::string result(len, '0');
        for (size_t i = 0, j = (len - 1) * 4; i < len; ++i, j -= 4)
            result[i] = digits[(n >> j) & 0x0f];

        return result;
    }

    inline std::vector<std::string> explodeString(const std::string& str, char delimiter = ' ')
    {
        std::string buffer;
        std::vector<std::string> result;

        for(char c : str)
            if (c != delimiter) buffer.push_back(c);
            else if (c == delimiter && !buffer.empty())
            {
                result.push_back(buffer);
                buffer.clear();
            }

        if (!buffer.empty()) result.push_back(buffer);
        return result;
    }

    void setCurrentThreadName(const std::string& name);
    void setThreadPriority(std::thread& t, float priority, bool realtime);
}

#endif // OUZEL_UTILS_UTILS_HPP
