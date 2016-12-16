// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Color.h"
#include "Vector3.h"
#include "Vector4.h"

namespace ouzel
{
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
