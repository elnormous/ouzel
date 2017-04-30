// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Color.h"
#include "Vector3.h"
#include "Vector4.h"
#include "utils/Utils.h"

namespace ouzel
{
    Color::Color(const std::string& color)
    {
        if (!color.empty())
        {
            uint32_t intValue = 0;

            if (color[0] == '#')
            {
                std::string newValue = "0x";
                newValue.append(color.begin() + 1, color.end());
                intValue = static_cast<uint32_t>(stringToLong(newValue));
            }
            else
            {
                intValue = static_cast<uint32_t>(stringToLong(color));
            }

            v[0] = static_cast<uint8_t>((intValue & 0xFF000000) >> 24);
            v[1] = static_cast<uint8_t>((intValue & 0x00FF0000) >> 16);
            v[2] = static_cast<uint8_t>((intValue & 0x0000FF00) >> 8);
            v[3] = static_cast<uint8_t>(intValue & 0x000000FF);
        }
    }

    Color& Color::operator=(const std::string& color)
    {
        if (!color.empty())
        {
            uint32_t intValue = 0;

            if (color[0] == '#')
            {
                std::string newValue = "0x";
                newValue.append(color.begin() + 1, color.end());
                intValue = static_cast<uint32_t>(stringToLong(newValue));
            }
            else
            {
                intValue = static_cast<uint32_t>(stringToLong(color));
            }

            v[0] = static_cast<uint8_t>((intValue & 0xFF000000) >> 24);
            v[1] = static_cast<uint8_t>((intValue & 0x00FF0000) >> 16);
            v[2] = static_cast<uint8_t>((intValue & 0x0000FF00) >> 8);
            v[3] = static_cast<uint8_t>(intValue & 0x000000FF);
        }

        return *this;
    }

    Color::Color(const Vector3& vec)
    {
        v[0] = static_cast<uint8_t>(vec.v[0] * 255.0f);
        v[1] = static_cast<uint8_t>(vec.v[1] * 255.0f);
        v[2] = static_cast<uint8_t>(vec.v[2] * 255.0f);
        v[3] = 0;
    }

    Color& Color::operator=(const Vector3& vec)
    {
        v[0] = static_cast<uint8_t>(vec.v[0] * 255.0f);
        v[1] = static_cast<uint8_t>(vec.v[1] * 255.0f);
        v[2] = static_cast<uint8_t>(vec.v[2] * 255.0f);
        v[3] = 0;

        return *this;
    }

    Color::Color(const Vector4& vec)
    {
        v[0] = static_cast<uint8_t>(vec.v[0] * 255.0f);
        v[1] = static_cast<uint8_t>(vec.v[1] * 255.0f);
        v[2] = static_cast<uint8_t>(vec.v[2] * 255.0f);
        v[3] = static_cast<uint8_t>(vec.v[3] * 255.0f);
    }

    Color& Color::operator=(const Vector4& vec)
    {
        v[0] = static_cast<uint8_t>(vec.v[0] * 255.0f);
        v[1] = static_cast<uint8_t>(vec.v[1] * 255.0f);
        v[2] = static_cast<uint8_t>(vec.v[2] * 255.0f);
        v[3] = static_cast<uint8_t>(vec.v[3] * 255.0f);

        return *this;
    }
} // namespace ouzel
