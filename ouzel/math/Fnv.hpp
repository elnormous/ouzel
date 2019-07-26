// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_FNV_HPP
#define OUZEL_MATH_FNV_HPP

#include <cstdint>

namespace ouzel
{
    namespace fnv
    {
        template <typename T> T prime;
        template <typename T> T offsetBasis;

        template <> constexpr uint32_t prime<uint32_t> = 16777619u;
        template <> constexpr uint32_t offsetBasis<uint32_t> = 2166136261u;
        template <> constexpr uint64_t prime<uint64_t> = 1099511628211u;
        template <> constexpr uint64_t offsetBasis<uint64_t> = 14695981039346656037u;

        // Fowler / Noll / Vo (FNV) hash
        template <typename Result, typename Value> inline auto hash(const Value value) noexcept
        {
            Result result = offsetBasis<Result>;
            for (size_t i = 0; i < sizeof(Value); ++i)
                result = (result ^ static_cast<uint8_t>(value >> (i * 8))) * prime<Result>;
            return result;
        }
    } // namespace fnv
} // namespace ouzel

#endif // OUZEL_MATH_FNV_HPP
