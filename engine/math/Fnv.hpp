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
            template <typename T> T prime();
            template <typename T> T offsetBasis();

            template <> constexpr std::uint32_t prime<std::uint32_t>() { return 16777619u; }
            template <> constexpr std::uint32_t offsetBasis<std::uint32_t>() { return 2166136261u; }
                template <> constexpr std::uint64_t prime<std::uint64_t>() { return 1099511628211u; }
                template <> constexpr std::uint64_t offsetBasis<std::uint64_t>() { return 14695981039346656037u; }
        }

        // Fowler / Noll / Vo (FNV) hash
        template <typename Result, typename Value>
        constexpr Result hash(const Value value, std::size_t i = 0, Result result = offsetBasis<Result>()) noexcept
        {
            return (i < sizeof(Value)) ? hash<Result>(value, i + 1, (result * prime<Result>()) ^ ((value >> (i * 8)) & 0xFF)) : result;
        }
    } // namespace fnv
} // namespace ouzel

#endif // OUZEL_MATH_FNV_HPP
