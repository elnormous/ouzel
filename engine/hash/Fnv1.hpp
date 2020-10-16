// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_HASH_FNV1_HPP
#define OUZEL_HASH_FNV1_HPP

#include <cstdint>

namespace ouzel::hash::fnv1
{
    inline namespace detail
    {
        template <typename T> struct prime;
        template <typename T> struct offsetBasis;

        template <> struct prime<std::uint32_t>
        {
            static constexpr std::uint32_t value = 16777619U;
        };

        template <> struct offsetBasis<std::uint32_t>
        {
            static constexpr std::uint32_t value = 2166136261U;
        };

        template <> struct prime<std::uint64_t>
        {
            static constexpr std::uint64_t value = 1099511628211ULL;
        };

        template <> struct offsetBasis<std::uint64_t>
        {
            static constexpr std::uint64_t value = 14695981039346656037ULL;
        };
    }

    template <typename Result, typename Value>
    constexpr Result hash(const Value value, const std::size_t i = 0,
                          const Result result = offsetBasis<Result>::value) noexcept
    {
        return (i < sizeof(Value)) ? hash<Result>(value, i + 1, (result * prime<Result>::value) ^ ((value >> (i * 8)) & 0xFF)) : result;
    }
}

#endif // OUZEL_HASH_FNV1_HPP
