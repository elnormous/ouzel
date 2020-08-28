// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_HASH_FNV1_HPP
#define OUZEL_HASH_FNV1_HPP

#include <cstdint>

namespace ouzel::hash::fnv1
{
    inline namespace detail
    {
        template <typename T> T prime();
        template <typename T> T offsetBasis();

        template <> constexpr std::uint32_t prime<std::uint32_t>() { return 16777619U; }
        template <> constexpr std::uint32_t offsetBasis<std::uint32_t>() { return 2166136261U; }
        template <> constexpr std::uint64_t prime<std::uint64_t>() { return 1099511628211ULL; }
        template <> constexpr std::uint64_t offsetBasis<std::uint64_t>() { return 14695981039346656037ULL; }
    }

    // Fowler / Noll / Vo (FNV) hash
    template <typename Result, typename Value>
    constexpr Result hash(const Value value, std::size_t i = 0, Result result = offsetBasis<Result>()) noexcept
    {
        return (i < sizeof(Value)) ? hash<Result>(value, i + 1, (result * prime<Result>()) ^ ((value >> (i * 8)) & 0xFF)) : result;
    }
}

#endif // OUZEL_HASH_FNV1_HPP
