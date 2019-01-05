// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Color.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    static uint32_t parseColorString(const std::string& value)
    {
        uint32_t result = 0;

        if (!value.empty())
        {
            if (value.front() == '#')
            {
                std::string newValue(value.begin() + 1, value.end());

                size_t componentSize = (newValue.length() + 2) / 3; // get the size of component rounded up
                size_t newSize = componentSize * 3;
                newValue.resize(newSize);

                for (size_t component = 0; component < 3; ++component)
                {
                    std::string currentValue;

                    for (size_t byte = 0; byte < ((componentSize < 2) ? componentSize : 2); ++byte)
                    {
                        char c = newValue[component * componentSize + byte];

                        if ((c >= '0' && c <= '9') ||
                            (c >= 'a' && c <= 'f') ||
                            (c >= 'A' && c <= 'F'))
                            currentValue += c;
                        else
                            currentValue += "0";
                    }

                    result |= (static_cast<uint32_t>(std::stoul(currentValue, 0, 16))) << ((3 - component) * 8);
                }

                result |= 0xff; // alpha
            }
            else
                result = static_cast<uint32_t>(std::stoul(value));
        }

        return result;
    }

    Color::Color(const std::string& color)
    {
        uint32_t intValue = parseColorString(color);

        r = static_cast<uint8_t>((intValue & 0xFF000000) >> 24);
        g = static_cast<uint8_t>((intValue & 0x00FF0000) >> 16);
        b = static_cast<uint8_t>((intValue & 0x0000FF00) >> 8);
        a = static_cast<uint8_t>(intValue & 0x000000FF);
    }

    Color& Color::operator=(const std::string& color)
    {
        uint32_t intValue = parseColorString(color);

        r = static_cast<uint8_t>((intValue & 0xFF000000) >> 24);
        g = static_cast<uint8_t>((intValue & 0x00FF0000) >> 16);
        b = static_cast<uint8_t>((intValue & 0x0000FF00) >> 8);
        a = static_cast<uint8_t>(intValue & 0x000000FF);

        return *this;
    }

    
    Color::Color(const Vector3<float>& vec)
    {
        r = static_cast<uint8_t>(vec.v[0] * 255.0F);
        g = static_cast<uint8_t>(vec.v[1] * 255.0F);
        b = static_cast<uint8_t>(vec.v[2] * 255.0F);
        a = 0;
    }

    Color& Color::operator=(const Vector3<float>& vec)
    {
        r = static_cast<uint8_t>(vec.v[0] * 255.0F);
        g = static_cast<uint8_t>(vec.v[1] * 255.0F);
        b = static_cast<uint8_t>(vec.v[2] * 255.0F);
        a = 0;

        return *this;
    }

    Color::Color(const Vector4<float>& vec)
    {
        r = static_cast<uint8_t>(vec.v[0] * 255.0F);
        g = static_cast<uint8_t>(vec.v[1] * 255.0F);
        b = static_cast<uint8_t>(vec.v[2] * 255.0F);
        a = static_cast<uint8_t>(vec.v[3] * 255.0F);
    }

    Color& Color::operator=(const Vector4<float>& vec)
    {
        r = static_cast<uint8_t>(vec.v[0] * 255.0F);
        g = static_cast<uint8_t>(vec.v[1] * 255.0F);
        b = static_cast<uint8_t>(vec.v[2] * 255.0F);
        a = static_cast<uint8_t>(vec.v[3] * 255.0F);

        return *this;
    }
} // namespace ouzel
