// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Vector3.h"

namespace ouzel
{
    class Size2;

    class Size3
    {
    public:
        float v[3];

        Size3()
        {
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 0.0f;
        }

        Size3(float w, float h, float d)
        {
            v[0] = w;
            v[1] = h;
            v[2] = d;
        }

        Size3(const Vector3& point)
        {
            v[0] = point.v[0];
            v[1] = point.v[1];
            v[2] = point.v[2];
        }

        Size3& operator=(const Vector3& point)
        {
            v[0] = point.v[0];
            v[1] = point.v[1];
            v[2] = point.v[2];
            return *this;
        }

        Size3(const Size2& s);
        Size3& operator=(const Size2& s);

        float& width() { return v[0]; }
        float& height() { return v[1]; }
        float& depth() { return v[2]; }
        float width() const { return v[0]; }
        float height() const { return v[1]; }
        float depth() const { return v[2]; }
        float& operator[](size_t index) { return v[index]; }
        float operator[](size_t index) const { return v[index]; }

        Size3 operator+(const Size3& size) const
        {
            return Size3(v[0] + size.v[0], v[1] + size.v[1], v[2] + size.v[2]);
        }

        Size3 operator-(const Size3& size) const
        {
            return Size3(v[0] - size.v[0], v[1] - size.v[1], v[2] - size.v[2]);
        }

        Size3 operator*(float a) const
        {
            return Size3(v[0] * a, v[1] * a, v[2] * a);
        }

        Size3 operator/(float a) const
        {
            return Size3(v[0] / a, v[1] / a, v[2] / a);
        }

        inline bool operator==(const Size3& size) const
        {
            return v[0] == size.v[0] && v[1] == size.v[1] && v[2] == size.v[2];
        }

        inline bool operator!=(const Size3& size) const
        {
            return v[0] != size.v[0] || v[1] != size.v[1] || v[2] != size.v[2];
        }

        bool isZero() const
        {
            return v[0] == 0.0f && v[1] == 0.0f && v[2] == 0.0f;
        }

        bool isOne() const
        {
            return v[0] == 1.0f && v[1] == 1.0f && v[2] == 1.0f;
        }

        float volume() const
        {
            return v[0] * v[1] * v[2];
        }
    };

    inline Size3 operator*(const Size3& size, const Vector3& v)
    {
        return Size3(size.v[0] * v.v[0], size.v[1] * v.v[1], size.v[2] * v.v[2]);
    }

    inline Size3 operator/(const Size3& size, const Vector3& v)
    {
        return Size3(size.v[0] / v.v[0], size.v[1] / v.v[1], size.v[2] / v.v[2]);
    }
}
