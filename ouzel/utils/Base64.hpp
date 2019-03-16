// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_BASE64_HPP
#define OUZEL_BASE64_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace ouzel
{
    namespace base64
    {
        static const std::string CHARS =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";


        static inline bool isBase64(unsigned char c)
        {
            return (isalnum(c) || (c == '+') || (c == '/'));
        }

        inline std::string encode(const std::vector<uint8_t>& bytes)
        {
            std::string result;
            int i = 0;
            int j = 0;
            uint8_t charArray3[3];
            uint8_t charArray4[4];
            const uint8_t* bytesToEncode = bytes.data();

            for (int l = 0; l < bytes.size(); ++l)
            {
                charArray3[i++] = *(bytesToEncode++);
                if (i == 3)
                {
                    charArray4[0] = (charArray3[0] & 0xFC) >> 2;
                    charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xF0) >> 4);
                    charArray4[2] = ((charArray3[1] & 0x0F) << 2) + ((charArray3[2] & 0xC0) >> 6);
                    charArray4[3] = charArray3[2] & 0x3f;

                    for(i = 0; (i <4) ; i++) result += CHARS[charArray4[i]];
                    i = 0;
                }
            }

            if (i)
            {
                for (j = i; j < 3; j++) charArray3[j] = '\0';

                charArray4[0] = (charArray3[0] & 0xFC) >> 2;
                charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xF0) >> 4);
                charArray4[2] = ((charArray3[1] & 0x0F) << 2) + ((charArray3[2] & 0xC0) >> 6);

                for (j = 0; (j < i + 1); j++) result += CHARS[charArray4[j]];

                while (i++ < 3) result += '=';

            }

            return result;
        }

        inline std::vector<uint8_t> decode(const std::string& encodedString)
        {
            int i = 0;
            int j = 0;
            int in = 0;
            uint8_t charArray3[3];
            uint8_t charArray4[4];
            std::vector<uint8_t> result;

            for (int l = 0; l < encodedString.size() && encodedString[in] != '=' && isBase64(encodedString[in]); ++l)
            {
                charArray4[i++] = encodedString[in]; in++;
                if (i == 4)
                {
                    for (i = 0; i <4; i++) charArray4[i] = CHARS.find(charArray4[i]);

                    charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
                    charArray3[1] = ((charArray4[1] & 0x0F) << 4) + ((charArray4[2] & 0x3C) >> 2);
                    charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

                    for (i = 0; (i < 3); i++) result.push_back(charArray3[i]);
                    i = 0;
                }
            }

            if (i)
            {
                for (j = 0; j < i; j++) charArray4[j] = CHARS.find(charArray4[j]);

                charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
                charArray3[1] = ((charArray4[1] & 0x0F) << 4) + ((charArray4[2] & 0x3C) >> 2);

                for (j = 0; (j < i - 1); j++) result.push_back(charArray3[j]);
            }

            return result;
        }
    } // namespace base64
} // namespace ouzel

#endif // OUZEL_BASE64_HPP
