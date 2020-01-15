// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_FNV_HPP
#define OUZEL_MATH_FNV_HPP

#include <cstdint>

namespace ouzel
{
    namespace fnv
    {
        inline namespace detail
        {
            template <typename T> constexpr T prime = T(0);
            template <typename T> constexpr T offsetBasis = T(0);

            template <> constexpr uint32_t prime<uint32_t> = 16777619u;
            template <> constexpr uint32_t offsetBasis<uint32_t> = 2166136261u;
            template <> constexpr uint64_t prime<uint64_t> = 1099511628211u;
            template <> constexpr uint64_t offsetBasis<uint64_t> = 14695981039346656037u;
        }

        // Fowler / Noll / Vo (FNV) hash
        template <typename Result, typename Value>
        constexpr Result hash(const Value value, size_t i = 0, Result result = offsetBasis<Result>) noexcept
        {
            return (i < sizeof(Value)) ? hash<Result>(value, i + 1, (result * prime<Result>) ^ ((value >> (i * 8)) & 0xFF)) : result;
        }
    } // namespace fnv
} // namespace ouzel

#endif // OUZEL_MATH_FNV_HPP
