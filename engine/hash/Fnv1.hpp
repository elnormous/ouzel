// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_HASH_FNV1_HPP
#define OUZEL_HASH_FNV1_HPP

#include <cstdint>

namespace ouzel::hash::fnv1
{
    inline namespace detail
    {
        template <typename T> struct Constants;

        template <> struct Constants<std::uint32_t>
        {
            static constexpr std::uint32_t prime = 16777619U;
            static constexpr std::uint32_t offsetBasis = 2166136261U;
        };

        template <> struct Constants<std::uint64_t>
        {
            static constexpr std::uint64_t prime = 1099511628211ULL;
            static constexpr std::uint64_t offsetBasis = 14695981039346656037ULL;
        };
    }

    template <typename Result, typename Value>
    constexpr Result hash(const Value value, const std::size_t i = 0,
                          const Result result = Constants<Result>::offsetBasis) noexcept
    {
        return (i < sizeof(Value)) ? hash<Result>(value, i + 1, (result * Constants<Result>::prime) ^ ((value >> (i * 8)) & 0xFF)) : result;
    }
}

#endif // OUZEL_HASH_FNV1_HPP
