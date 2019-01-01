// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_UTILS_UTILS_HPP
#define OUZEL_UTILS_UTILS_HPP

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <limits>
#include <random>
#include <string>
#include <vector>

#if defined(__ANDROID__)
#  include <cpu-features.h>
#endif

namespace ouzel
{
#if defined(__ARM_NEON__)
#  if defined(__ANDROID__) && defined(__arm__)
    // NEON support must be checked at runtime on 32-bit Android
    class AnrdoidNeonChecker
    {
    public:
        AnrdoidNeonChecker()
        {
            neonAvailable = (android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM &&
                             (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0);
        }

        operator bool() const { return neonAvailable; }

    private:
        bool neonAvailable;
    };

    extern AnrdoidNeonChecker isSimdAvailable;
#  else
    constexpr bool isSimdAvailable = true;
#  endif
#elif defined(__SSE__)
    constexpr bool isSimdAvailable = true;
#else
    constexpr bool isSimdAvailable = false;
#endif

    extern std::mt19937 randomEngine;

    template<typename T>
    size_t getVectorSize(const typename std::vector<T>& vec)
    {
        return sizeof(T) * vec.size();
    }

    // decoding

    inline int64_t decodeInt64Big(const void* buffer)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(buffer);

        return static_cast<int64_t>(bytes[7]) |
            static_cast<int64_t>(bytes[6]) << 8 |
            static_cast<int64_t>(bytes[5]) << 16 |
            static_cast<int64_t>(bytes[4]) << 24 |
            static_cast<int64_t>(bytes[3]) << 32 |
            static_cast<int64_t>(bytes[2]) << 40 |
            static_cast<int64_t>(bytes[1]) << 48 |
            static_cast<int64_t>(bytes[0]) << 56;
    };

    inline uint64_t decodeUInt64Big(const void* buffer)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(buffer);

        return static_cast<uint64_t>(bytes[7]) |
            static_cast<uint64_t>(bytes[6]) << 8 |
            static_cast<uint64_t>(bytes[5]) << 16 |
            static_cast<uint64_t>(bytes[4]) << 24 |
            static_cast<uint64_t>(bytes[3]) << 32 |
            static_cast<uint64_t>(bytes[2]) << 40 |
            static_cast<uint64_t>(bytes[1]) << 48 |
            static_cast<uint64_t>(bytes[0]) << 56;
    };

    inline int64_t decodeInt64Little(const void* buffer)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(buffer);

        return static_cast<int64_t>(bytes[0]) |
            static_cast<int64_t>(bytes[1]) << 8 |
            static_cast<int64_t>(bytes[2]) << 16 |
            static_cast<int64_t>(bytes[3]) << 24 |
            static_cast<int64_t>(bytes[4]) << 32 |
            static_cast<int64_t>(bytes[5]) << 40 |
            static_cast<int64_t>(bytes[6]) << 48 |
            static_cast<int64_t>(bytes[7]) << 56;
    };

    inline uint64_t decodeUInt64Little(const void* buffer)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(buffer);

        return static_cast<uint64_t>(bytes[0]) |
            static_cast<uint64_t>(bytes[1]) << 8 |
            static_cast<uint64_t>(bytes[2]) << 16 |
            static_cast<uint64_t>(bytes[3]) << 24 |
            static_cast<uint64_t>(bytes[4]) << 32 |
            static_cast<uint64_t>(bytes[5]) << 40 |
            static_cast<uint64_t>(bytes[6]) << 48 |
            static_cast<uint64_t>(bytes[7]) << 56;
    };

    inline int32_t decodeInt32Big(const void* buffer)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(buffer);

        return static_cast<int32_t>(bytes[3]) |
            static_cast<int32_t>(bytes[2]) << 8 |
            static_cast<int32_t>(bytes[1]) << 16 |
            static_cast<int32_t>(bytes[0]) << 24;
    };

    inline uint32_t decodeUInt32Big(const void* buffer)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(buffer);

        return static_cast<uint32_t>(bytes[3]) |
            static_cast<uint32_t>(bytes[2]) << 8 |
            static_cast<uint32_t>(bytes[1]) << 16 |
            static_cast<uint32_t>(bytes[0]) << 24;
    };

    inline int32_t decodeInt32Little(const void* buffer)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(buffer);

        return static_cast<int32_t>(bytes[0]) |
            static_cast<int32_t>(bytes[1]) << 8 |
            static_cast<int32_t>(bytes[2]) << 16 |
            static_cast<int32_t>(bytes[3]) << 24;
    };

    inline uint32_t decodeUInt32Little(const void* buffer)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(buffer);

        return static_cast<uint32_t>(bytes[0]) |
            static_cast<uint32_t>(bytes[1]) << 8 |
            static_cast<uint32_t>(bytes[2]) << 16 |
            static_cast<uint32_t>(bytes[3]) << 24;
    };

    inline int16_t decodeInt16Big(const void* buffer)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(buffer);

        int32_t result = static_cast<int32_t>(bytes[1]) |
            static_cast<int32_t>(bytes[0]) << 8;

        return static_cast<int16_t>(result);
    };

    inline uint16_t decodeUInt16Big(const void* buffer)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(buffer);

        uint32_t result = static_cast<uint32_t>(bytes[1]) |
            static_cast<uint32_t>(bytes[0]) << 8;

        return static_cast<uint16_t>(result);
    };

    inline int16_t decodeInt16Little(const void* buffer)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(buffer);

        int32_t result = static_cast<int32_t>(bytes[0]) |
            static_cast<int32_t>(bytes[1]) << 8;

        return static_cast<int16_t>(result);
    };

    inline uint16_t decodeUInt16Little(const void* buffer)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(buffer);

        uint32_t result = static_cast<uint32_t>(bytes[0]) |
            static_cast<uint32_t>(bytes[1]) << 8;

        return static_cast<uint16_t>(result);
    };

    // encoding

    inline void encodeInt64Big(void* buffer, int64_t value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        bytes[0] = static_cast<uint8_t>(value >> 56);
        bytes[1] = static_cast<uint8_t>(value >> 48);
        bytes[2] = static_cast<uint8_t>(value >> 40);
        bytes[3] = static_cast<uint8_t>(value >> 32);
        bytes[4] = static_cast<uint8_t>(value >> 24);
        bytes[5] = static_cast<uint8_t>(value >> 16);
        bytes[6] = static_cast<uint8_t>(value >> 8);
        bytes[7] = static_cast<uint8_t>(value);
    };

    inline void encodeUInt64Big(void* buffer, uint64_t value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        bytes[0] = static_cast<uint8_t>(value >> 56);
        bytes[1] = static_cast<uint8_t>(value >> 48);
        bytes[2] = static_cast<uint8_t>(value >> 40);
        bytes[3] = static_cast<uint8_t>(value >> 32);
        bytes[4] = static_cast<uint8_t>(value >> 24);
        bytes[5] = static_cast<uint8_t>(value >> 16);
        bytes[6] = static_cast<uint8_t>(value >> 8);
        bytes[7] = static_cast<uint8_t>(value);
    };

    inline void encodeInt64Little(void* buffer, int64_t value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        bytes[7] = static_cast<uint8_t>(value >> 56);
        bytes[6] = static_cast<uint8_t>(value >> 48);
        bytes[5] = static_cast<uint8_t>(value >> 40);
        bytes[4] = static_cast<uint8_t>(value >> 32);
        bytes[3] = static_cast<uint8_t>(value >> 24);
        bytes[2] = static_cast<uint8_t>(value >> 16);
        bytes[1] = static_cast<uint8_t>(value >> 8);
        bytes[0] = static_cast<uint8_t>(value);
    };

    inline void encodeUInt64Little(void* buffer, uint64_t value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        bytes[7] = static_cast<uint8_t>(value >> 56);
        bytes[6] = static_cast<uint8_t>(value >> 48);
        bytes[5] = static_cast<uint8_t>(value >> 40);
        bytes[4] = static_cast<uint8_t>(value >> 32);
        bytes[3] = static_cast<uint8_t>(value >> 24);
        bytes[2] = static_cast<uint8_t>(value >> 16);
        bytes[1] = static_cast<uint8_t>(value >> 8);
        bytes[0] = static_cast<uint8_t>(value);
    };

    inline void encodeInt32Big(void* buffer, int32_t value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        bytes[0] = static_cast<uint8_t>(value >> 24);
        bytes[1] = static_cast<uint8_t>(value >> 16);
        bytes[2] = static_cast<uint8_t>(value >> 8);
        bytes[3] = static_cast<uint8_t>(value);
    };

    inline void encodeUInt32Big(void* buffer, uint32_t value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        bytes[0] = static_cast<uint8_t>(value >> 24);
        bytes[1] = static_cast<uint8_t>(value >> 16);
        bytes[2] = static_cast<uint8_t>(value >> 8);
        bytes[3] = static_cast<uint8_t>(value);
    };

    inline void encodeInt32Little(void* buffer, int32_t value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        bytes[3] = static_cast<uint8_t>(value >> 24);
        bytes[2] = static_cast<uint8_t>(value >> 16);
        bytes[1] = static_cast<uint8_t>(value >> 8);
        bytes[0] = static_cast<uint8_t>(value);
    };

    inline void encodeUInt32Little(void* buffer, uint32_t value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        bytes[3] = static_cast<uint8_t>(value >> 24);
        bytes[2] = static_cast<uint8_t>(value >> 16);
        bytes[1] = static_cast<uint8_t>(value >> 8);
        bytes[0] = static_cast<uint8_t>(value);
    };

    inline void encodeInt16Big(void* buffer, int16_t value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        bytes[0] = static_cast<uint8_t>(value >> 8);
        bytes[1] = static_cast<uint8_t>(value);
    };

    inline void encodeUInt16Big(void* buffer, uint16_t value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        bytes[0] = static_cast<uint8_t>(value >> 8);
        bytes[1] = static_cast<uint8_t>(value);
    };

    inline void encodeInt16Little(void* buffer, int16_t value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        bytes[1] = static_cast<uint8_t>(value >> 8);
        bytes[0] = static_cast<uint8_t>(value);
    };

    inline void encodeUInt16Little(void* buffer, uint16_t value)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);

        bytes[1] = static_cast<uint8_t>(value >> 8);
        bytes[0] = static_cast<uint8_t>(value);
    };

    template<typename T> std::string hexToString(T n, size_t len = 0)
    {
        static constexpr const char* digits = "0123456789ABCDEF";
        if (len == 0)
        {
            len = 1;
            while (static_cast<size_t>(1ull << (len * 4)) <= static_cast<size_t>(n)) ++len;
        }

        std::string result(len, '0');
        for (size_t i = 0, j = (len - 1) * 4 ; i < len; ++i, j -= 4)
            result[i] = digits[(n >> j) & 0x0f];

        return result;
    }

    void setCurrentThreadName(const std::string& name);
}

#endif // OUZEL_UTILS_UTILS_HPP
