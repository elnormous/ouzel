// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <functional>
#include <random>

#include "core/Setup.h"
#if OUZEL_PLATFORM_ANDROID
#include <cpu-features.h>
#endif

#define OUZEL_UNUSED(x) (void)(x)

namespace ouzel
{
#if OUZEL_PLATFORM_ANDROID && OUZEL_SUPPORTS_NEON_CHECK
    class AnrdoidNEONChecker
    {
    public:
        AnrdoidNEONChecker()
        {
            if (android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM && (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0)
                neonAvailable = true;
            else
                neonAvailable = false;
        }

        bool isNEONAvailable() const { return neonAvailable; }

    private:
        bool neonAvailable;
    };

    extern AnrdoidNEONChecker anrdoidNEONChecker;
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
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(buffer);

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
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(buffer);

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
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(buffer);

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
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(buffer);

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
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(buffer);

        return static_cast<int32_t>(bytes[3]) |
            static_cast<int32_t>(bytes[2]) << 8 |
            static_cast<int32_t>(bytes[1]) << 16 |
            static_cast<int32_t>(bytes[0]) << 24;
    };

    inline uint32_t decodeUInt32Big(const void* buffer)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(buffer);

        return static_cast<uint32_t>(bytes[3]) |
            static_cast<uint32_t>(bytes[2]) << 8 |
            static_cast<uint32_t>(bytes[1]) << 16 |
            static_cast<uint32_t>(bytes[0]) << 24;
    };

    inline int32_t decodeInt32Little(const void* buffer)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(buffer);

        return static_cast<int32_t>(bytes[0]) |
            static_cast<int32_t>(bytes[1]) << 8 |
            static_cast<int32_t>(bytes[2]) << 16 |
            static_cast<int32_t>(bytes[3]) << 24;
    };

    inline uint32_t decodeUInt32Little(const void* buffer)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(buffer);

        return static_cast<uint32_t>(bytes[0]) |
            static_cast<uint32_t>(bytes[1]) << 8 |
            static_cast<uint32_t>(bytes[2]) << 16 |
            static_cast<uint32_t>(bytes[3]) << 24;
    };

    inline int16_t decodeInt16Big(const void* buffer)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(buffer);

        int32_t result = static_cast<int32_t>(bytes[1]) |
            static_cast<int32_t>(bytes[0]) << 8;

        return static_cast<int16_t>(result);
    };

    inline uint16_t decodeUInt16Big(const void* buffer)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(buffer);

        uint32_t result = static_cast<uint32_t>(bytes[1]) |
            static_cast<uint32_t>(bytes[0]) << 8;

        return static_cast<uint16_t>(result);
    };

    inline int16_t decodeInt16Little(const void* buffer)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(buffer);

        int32_t result = static_cast<int32_t>(bytes[0]) |
            static_cast<int32_t>(bytes[1]) << 8;

        return static_cast<int16_t>(result);
    };

    inline uint16_t decodeUInt16Little(const void* buffer)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(buffer);

        uint32_t result = static_cast<uint32_t>(bytes[0]) |
            static_cast<uint32_t>(bytes[1]) << 8;

        return static_cast<uint16_t>(result);
    };

    // encoding

    inline void encodeInt64Big(void* buffer, int64_t value)
    {
        uint8_t* bytes = reinterpret_cast<uint8_t*>(buffer);

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
        uint8_t* bytes = reinterpret_cast<uint8_t*>(buffer);

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
        uint8_t* bytes = reinterpret_cast<uint8_t*>(buffer);

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
        uint8_t* bytes = reinterpret_cast<uint8_t*>(buffer);

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
        uint8_t* bytes = reinterpret_cast<uint8_t*>(buffer);

        bytes[0] = static_cast<uint8_t>(value >> 24);
        bytes[1] = static_cast<uint8_t>(value >> 16);
        bytes[2] = static_cast<uint8_t>(value >> 8);
        bytes[3] = static_cast<uint8_t>(value);
    };

    inline void encodeUInt32Big(void* buffer, uint32_t value)
    {
        uint8_t* bytes = reinterpret_cast<uint8_t*>(buffer);

        bytes[0] = static_cast<uint8_t>(value >> 24);
        bytes[1] = static_cast<uint8_t>(value >> 16);
        bytes[2] = static_cast<uint8_t>(value >> 8);
        bytes[3] = static_cast<uint8_t>(value);
    };

    inline void encodeInt32Little(void* buffer, int32_t value)
    {
        uint8_t* bytes = reinterpret_cast<uint8_t*>(buffer);

        bytes[3] = static_cast<uint8_t>(value >> 24);
        bytes[2] = static_cast<uint8_t>(value >> 16);
        bytes[1] = static_cast<uint8_t>(value >> 8);
        bytes[0] = static_cast<uint8_t>(value);
    };

    inline void encodeUInt32Little(void* buffer, uint32_t value)
    {
        uint8_t* bytes = reinterpret_cast<uint8_t*>(buffer);

        bytes[3] = static_cast<uint8_t>(value >> 24);
        bytes[2] = static_cast<uint8_t>(value >> 16);
        bytes[1] = static_cast<uint8_t>(value >> 8);
        bytes[0] = static_cast<uint8_t>(value);
    };

    inline void encodeInt16Big(void* buffer, int16_t value)
    {
        uint8_t* bytes = reinterpret_cast<uint8_t*>(buffer);

        bytes[0] = static_cast<uint8_t>(value >> 8);
        bytes[1] = static_cast<uint8_t>(value);
    };

    inline void encodeUInt16Big(void* buffer, uint16_t value)
    {
        uint8_t* bytes = reinterpret_cast<uint8_t*>(buffer);

        bytes[0] = static_cast<uint8_t>(value >> 8);
        bytes[1] = static_cast<uint8_t>(value);
    };

    inline void encodeInt16Little(void* buffer, int16_t value)
    {
        uint8_t* bytes = reinterpret_cast<uint8_t*>(buffer);

        bytes[1] = static_cast<uint8_t>(value >> 8);
        bytes[0] = static_cast<uint8_t>(value);
    };

    inline void encodeUInt16Little(void* buffer, uint16_t value)
    {
        uint8_t* bytes = reinterpret_cast<uint8_t*>(buffer);

        bytes[1] = static_cast<uint8_t>(value >> 8);
        bytes[0] = static_cast<uint8_t>(value);
    };

    template<class T>
    inline std::vector<uint32_t> utf8ToUtf32(const T& text)
    {
        std::vector<uint32_t> result;

        for (auto i = text.begin(); i != text.end(); ++i)
        {
            uint32_t cp = *i & 0xff;

            if (cp <= 0x7f) // length = 1
            {
                // do nothing
            }
            else if ((cp >> 5) == 0x6) // length = 2
            {
                if (++i == text.end()) return result;
                cp = ((cp << 6) & 0x7ff) + (*i & 0x3f);
            }
            else if ((cp >> 4) == 0xe) // length = 3
            {
                if (++i == text.end()) return result;
                cp = ((cp << 12) & 0xffff) + (((*i & 0xff) << 6) & 0xfff);
                if (++i == text.end()) return result;
                cp += *i & 0x3f;
            }
            else if ((cp >> 3) == 0x1e) // length = 4
            {
                if (++i == text.end()) return result;
                cp = ((cp << 18) & 0x1fffff) + (((*i & 0xff) << 12) & 0x3ffff);
                if (++i == text.end()) return result;
                cp += ((*i & 0xff) << 6) & 0xfff;
                if (++i == text.end()) return result;
                cp += (*i) & 0x3f;
            }

            result.push_back(cp);
        }

        return result;
    }

    inline std::string utf32ToUtf8(uint32_t c)
    {
        std::string result;

        if (c <= 0x7f)
            result.push_back(static_cast<char>(c));
        else if (c <= 0x7ff)
        {
            result.push_back(static_cast<char>(0xc0 | ((c >> 6) & 0x1f)));
            result.push_back(static_cast<char>(0x80 | (c & 0x3f)));
        }
        else if (c <= 0xffff)
        {
            result.push_back(static_cast<char>(0xe0 | ((c >> 12) & 0x0f)));
            result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3f)));
            result.push_back(static_cast<char>(0x80 | (c & 0x3f)));
        }
        else
        {
            result.push_back(static_cast<char>(0xf0 | ((c >> 18) & 0x07)));
            result.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3f)));
            result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3f)));
            result.push_back(static_cast<char>(0x80 | (c & 0x3f)));
        }

        return result;
    }

    inline std::string utf32ToUtf8(const std::vector<uint32_t>& text)
    {
        std::string result;

        for (auto i = text.begin(); i != text.end(); ++i)
        {
            if (*i <= 0x7f)
                result.push_back(static_cast<char>(*i));
            else if (*i <= 0x7ff)
            {
                result.push_back(static_cast<char>(0xc0 | ((*i >> 6) & 0x1f)));
                result.push_back(static_cast<char>(0x80 | (*i & 0x3f)));
            }
            else if (*i <= 0xffff)
            {
                result.push_back(static_cast<char>(0xe0 | ((*i >> 12) & 0x0f)));
                result.push_back(static_cast<char>(0x80 | ((*i >> 6) & 0x3f)));
                result.push_back(static_cast<char>(0x80 | (*i & 0x3f)));
            }
            else
            {
                result.push_back(static_cast<char>(0xf0 | ((*i >> 18) & 0x07)));
                result.push_back(static_cast<char>(0x80 | ((*i >> 12) & 0x3f)));
                result.push_back(static_cast<char>(0x80 | ((*i >> 6) & 0x3f)));
                result.push_back(static_cast<char>(0x80 | (*i & 0x3f)));
            }
        }

        return result;
    }

    template<typename T> std::string hexToString(T n, size_t len = 0)
    {
        static const char* digits = "0123456789ABCDEF";
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
}
