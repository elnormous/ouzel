// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

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
    auto getVectorSize(const T& vec) noexcept
    {
        return sizeof(typename T::value_type) * vec.size();
    }

    template <typename T, typename Iterator, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    auto decodeBigEndian(Iterator iterator) noexcept
    {
        T result = T(0);

        for (std::uintptr_t i = 0; i < sizeof(T); ++i, ++iterator)
            result |= static_cast<T>(static_cast<std::uint8_t>(*iterator) << ((sizeof(T) - i - 1) * 8));

        return result;
    }

    template <typename T, typename Iterator, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    auto decodeLittleEndian(Iterator iterator) noexcept
    {
        T result = T(0);

        for (std::uintptr_t i = 0; i < sizeof(T); ++i, ++iterator)
            result |= static_cast<T>(static_cast<std::uint8_t>(*iterator) << (i * 8));

        return result;
    }

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    void encodeBigEndian(std::uint8_t* buffer, const T value) noexcept
    {
        for (std::uintptr_t i = 0; i < sizeof(T); ++i)
            buffer[i] = static_cast<std::uint8_t>(value >> ((sizeof(T) - i - 1) * 8));
    }

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    void encodeLittleEndian(std::uint8_t* buffer, const T value) noexcept
    {
        for (std::uintptr_t i = 0; i < sizeof(T); ++i)
            buffer[i] = static_cast<std::uint8_t>(value >> (i * 8));
    }

    inline auto explodeString(const std::string& str, char delimiter = ' ')
    {
        std::vector<std::string> result;
        for (std::size_t position = 0, beginPosition = 0; position != std::string::npos; beginPosition = position + 1)
        {
            position = str.find(delimiter, beginPosition);
            const std::size_t endPosition = (position == std::string::npos) ? str.size() : position;
            if (endPosition != beginPosition)
                result.push_back(str.substr(beginPosition, endPosition - beginPosition));
        }
        return result;
    }
}

#endif // OUZEL_UTILS_UTILS_HPP
