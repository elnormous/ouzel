// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZE_UTILS_BASE64_HPP
#define OUZE_UTILS_BASE64_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace ouzel
{
    namespace base64
    {
        static const char CHARS[] = {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
            'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
            'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
        };

        constexpr bool isBase64(char c)
        {
            return ((c >= 'A' && c <= 'Z') ||
                    (c >= 'a' && c <= 'z') ||
                    (c >= '0' && c <= '9') ||
                    (c == '+') || (c == '/'));
        }

        constexpr uint8_t getIndex(uint8_t c)
        {
            return (c >= 'A' && c <= 'Z') ? c - 'A' :
                (c >= 'a' && c <= 'z') ? 26 + (c - 'a') :
                (c >= '0' && c <= '9') ? 52 + (c - '0') :
                (c == '+') ? 62 : (c == '/') ? 63 : 0;
        }

        inline std::string encode(const std::vector<uint8_t>& bytes)
        {
            std::string result;
            int i = 0;
            int j = 0;
            uint8_t charArray3[3];
            uint8_t charArray4[4];
            const uint8_t* bytesToEncode = bytes.data();

            for (uint32_t l = 0; l < bytes.size(); ++l)
            {
                charArray3[i++] = *(bytesToEncode++);
                if (i == 3)
                {
                    charArray4[0] = static_cast<uint8_t>((charArray3[0] & 0xFC) >> 2);
                    charArray4[1] = static_cast<uint8_t>(((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xF0) >> 4));
                    charArray4[2] = static_cast<uint8_t>(((charArray3[1] & 0x0F) << 2) + ((charArray3[2] & 0xC0) >> 6));
                    charArray4[3] = static_cast<uint8_t>(charArray3[2] & 0x3f);

                    for (i = 0; (i < 4) ; i++) result += CHARS[charArray4[i]];
                    i = 0;
                }
            }

            if (i)
            {
                for (j = i; j < 3; j++) charArray3[j] = '\0';

                charArray4[0] = static_cast<uint8_t>((charArray3[0] & 0xFC) >> 2);
                charArray4[1] = static_cast<uint8_t>(((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xF0) >> 4));
                charArray4[2] = static_cast<uint8_t>(((charArray3[1] & 0x0F) << 2) + ((charArray3[2] & 0xC0) >> 6));

                for (j = 0; (j < i + 1); j++) result += CHARS[charArray4[j]];

                while (i++ < 3) result += '=';

            }

            return result;
        }

        inline std::vector<uint8_t> decode(const std::string& encodedString)
        {
            uint32_t i = 0;
            uint32_t in = 0;
            uint8_t charArray3[3];
            uint8_t charArray4[4];
            std::vector<uint8_t> result;

            for (uint32_t l = 0; l < encodedString.size() && encodedString[in] != '='; ++l)
            {
                charArray4[i++] = static_cast<uint8_t>(encodedString[in]); in++;
                if (i == 4)
                {
                    for (i = 0; i < 4; i++) charArray4[i] = getIndex(charArray4[i]);

                    charArray3[0] = static_cast<uint8_t>((charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4));
                    charArray3[1] = static_cast<uint8_t>(((charArray4[1] & 0x0F) << 4) + ((charArray4[2] & 0x3C) >> 2));
                    charArray3[2] = static_cast<uint8_t>(((charArray4[2] & 0x3) << 6) + charArray4[3]);

                    for (i = 0; (i < 3); i++) result.push_back(charArray3[i]);
                    i = 0;
                }
            }

            if (i)
            {
                for (uint32_t j = 0; j < i; j++) charArray4[j] = getIndex(charArray4[j]);

                charArray3[0] = static_cast<uint8_t>((charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4));
                charArray3[1] = static_cast<uint8_t>(((charArray4[1] & 0x0F) << 4) + ((charArray4[2] & 0x3C) >> 2));

                for (uint32_t j = 0; (j < i - 1); j++) result.push_back(charArray3[j]);
            }

            return result;
        }
    } // namespace base64
} // namespace ouzel

#endif // OUZE_UTILS_BASE64_HPP
