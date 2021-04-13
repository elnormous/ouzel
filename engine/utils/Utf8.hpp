// Ouzel by Elviss Strazdins

#ifndef OUZEL_UTILS_UTF8_HPP
#define OUZEL_UTILS_UTF8_HPP

#include <cstdint>
#include <string>

namespace ouzel::utf8
{
    class ParseError final: public std::logic_error
    {
    public:
        using std::logic_error::logic_error;
    };

    template <class Iterator>
    std::u32string toUtf32(Iterator begin, Iterator end)
    {
        std::u32string result;

        for (auto i = begin; i != end; ++i)
        {
            char32_t cp = static_cast<char32_t>(*i) & 0xFF;

            if (cp <= 0x7F) // length = 1
            {
                // do nothing
            }
            else if ((cp >> 5) == 0x6) // length = 2
            {
                if (++i == end)
                    throw ParseError("Invalid UTF-8 string");
                cp = ((cp << 6) & 0x7FF) + (static_cast<char32_t>(*i) & 0x3F);
            }
            else if ((cp >> 4) == 0xE) // length = 3
            {
                if (++i == end)
                    throw ParseError("Invalid UTF-8 string");
                cp = ((cp << 12) & 0xFFFF) + (((static_cast<char32_t>(*i) & 0xFF) << 6) & 0x0FFF);
                if (++i == end)
                    throw ParseError("Invalid UTF-8 string");
                cp += static_cast<char32_t>(*i) & 0x3F;
            }
            else if ((cp >> 3) == 0x1E) // length = 4
            {
                if (++i == end)
                    throw ParseError("Invalid UTF-8 string");
                cp = ((cp << 18) & 0x1FFFFF) + (((static_cast<char32_t>(*i) & 0xFF) << 12) & 0x3FFFF);
                if (++i == end)
                    throw ParseError("Invalid UTF-8 string");
                cp += ((static_cast<char32_t>(*i) & 0xFF) << 6) & 0x0FFF;
                if (++i == end)
                    throw ParseError("Invalid UTF-8 string");
                cp += static_cast<char32_t>(*i) & 0x3F;
            }

            result.push_back(cp);
        }

        return result;
    }

    template <class T>
    std::u32string toUtf32(const T& text)
    {
        return toUtf32(std::begin(text), std::end(text));
    }

    inline std::string fromUtf32(char32_t c)
    {
        std::string result;

        if (c <= 0x7F)
            result.push_back(static_cast<char>(c));
        else if (c <= 0x7FF)
        {
            result.push_back(static_cast<char>(0xC0 | ((c >> 6) & 0x1F)));
            result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        }
        else if (c <= 0xFFFF)
        {
            result.push_back(static_cast<char>(0xE0 | ((c >> 12) & 0x0F)));
            result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        }
        else
        {
            result.push_back(static_cast<char>(0xF0 | ((c >> 18) & 0x07)));
            result.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        }

        return result;
    }

    template <class Iterator>
    std::string fromUtf32(Iterator begin, Iterator end)
    {
        std::string result;

        for (auto i = begin; i != end; ++i)
        {
            if (*i <= 0x7F)
                result.push_back(static_cast<char>(*i));
            else if (*i <= 0x7FF)
            {
                result.push_back(static_cast<char>(0xC0 | ((*i >> 6) & 0x1F)));
                result.push_back(static_cast<char>(0x80 | (*i & 0x3F)));
            }
            else if (*i <= 0xFFFF)
            {
                result.push_back(static_cast<char>(0xE0 | ((*i >> 12) & 0x0F)));
                result.push_back(static_cast<char>(0x80 | ((*i >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (*i & 0x3F)));
            }
            else
            {
                result.push_back(static_cast<char>(0xF0 | ((*i >> 18) & 0x07)));
                result.push_back(static_cast<char>(0x80 | ((*i >> 12) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | ((*i >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (*i & 0x3F)));
            }
        }

        return result;
    }

    template <class T>
    std::string fromUtf32(const T& text)
    {
        return fromUtf32(std::begin(text), std::end(text));
    }
}

#endif // OUZEL_UTILS_UTF8_HPP
