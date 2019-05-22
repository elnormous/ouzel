// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_UTILS_UTF8_HPP
#define OUZEL_UTILS_UTF8_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace ouzel
{
    namespace utf8
    {
        template <typename T>
        inline std::vector<uint32_t> toUtf32(const T& text)
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

        inline std::string fromUtf32(uint32_t c)
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

        inline std::string fromUtf32(const std::vector<uint32_t>& text)
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
    } // namespace json
} // namespace ouzel

#endif // OUZEL_UTILS_UTF8_HPP
