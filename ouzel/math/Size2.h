// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Vector2.h"

namespace ouzel
{
    class Size3;

    class Size2
    {
    public:
        float v[2];

        Size2()
        {
            v[0] = 0.0f;
            v[1] = 0.0f;
        }

        Size2(float w, float h)
        {
            v[0] = w;
            v[1] = h;
        }

        Size2(const Vector2& point)
        {
            v[0] = point.v[0];
            v[1] = point.v[1];
        }

        Size2& operator=(const Vector2& point)
        {
            v[0] = point.v[0];
            v[1] = point.v[1];

            return *this;
        }

        Size2(const Size3& s);
        Size2& operator=(const Size3& s);

        float& width() { return v[0]; }
        float& height() { return v[1]; }
        float width() const { return v[0]; }
        float height() const { return v[1]; }
        float& operator[](size_t index) { return v[index]; }
        float operator[](size_t index) const { return v[index]; }

        Size2 operator+(const Size2& size) const
        {
            return Size2(v[0] + size.v[0], v[1] + size.v[1]);
        }

        inline Size2& operator+=(const Size2& size)
        {
            v[0] += size.v[0];
            v[1] += size.v[1];
            return *this;
        }

        Size2 operator-(const Size2& size) const
        {
            return Size2(v[0] - size.v[0], v[1] - size.v[1]);
        }

        inline Size2& operator-=(const Size2& size)
        {
            v[0] -= size.v[0];
            v[1] -= size.v[1];
            return *this;
        }

        Size2 operator*(float a) const
        {
            return Size2(v[0] * a, v[1] * a);
        }

        inline Size2& operator*=(float scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
            return *this;
        }

        Size2 operator/(float a) const
        {
            return Size2(v[0] / a, v[1] / a);
        }

        inline Size2& operator/=(float scalar)
        {
            v[0] /= scalar;
            v[1] /= scalar;
            return *this;
        }

        inline bool operator==(const Size2& size) const
        {
            return v[0] == size.v[0] && v[1] == size.v[1];
        }

        inline bool operator!=(const Size2& size) const
        {
            return v[0] != size.v[0] || v[1] != size.v[1];
        }

        bool isZero() const
        {
            return v[0] == 0.0f && v[1] == 0.0f;
        }

        bool isOne() const
        {
            return v[0] == 1.0f && v[1] == 1.0f;
        }

        float area() const
        {
            return v[0] * v[1];
        }

        void scale(float scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
        }

        void scale(const Vector2& scale)
        {
            v[0] *= scale.v[0];
            v[1] *= scale.v[1];
        }
    };

    inline Size2 operator*(const Size2& size, const Vector2& v)
    {
        return Size2(size.v[0] * v.v[0], size.v[1] * v.v[1]);
    }

    inline Size2 operator/(const Size2& size, const Vector2& v)
    {
        return Size2(size.v[0] / v.v[0], size.v[1] / v.v[1]);
    }
}
