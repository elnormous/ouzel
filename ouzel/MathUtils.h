// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include "Vector2.h"

#define RANDOM_MINUS1_1()      ((2.0f * (static_cast<float>(rand()) / RAND_MAX)) - 1.0f) // Returns a random float between -1 and 1.
#define RANDOM_0_1()           (static_cast<float>(rand()) / RAND_MAX) // Returns a random float between 0 and 1.

namespace ouzel
{
    const float TAU = 6.28318530717958647692f;
    const float PI = 3.14159265358979323846f;
    const float PI_2 = 1.57079632679489661923f; // pi/2
    const float PI_4 = 0.78539816339744830962f; // pi/4
    const float FLOAT_SMALL = 1.0e-37f;
    const float TOLERANCE = 2e-37f;
    const float E = 2.71828182845904523536f;
    const float LOG10E = 0.4342944819032518f;
    const float LOG2E = 1.442695040888963387f;
    const float PIOVER2 = 1.57079632679489661923f;
    const float PIOVER4 = 0.785398163397448309616f;
    const float PIX2 = 6.28318530717958647693f;
    const float EPSILON = 0.000001f;

    uint32_t random(uint32_t min, uint32_t max);

    inline bool isPOT(int x)
    {
        return (x != 0) && (((x - 1) & x) == 0);
    }

    inline int nextPOT(int x)
    {
        x = x - 1;
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >>16);
        return x + 1;
    }

    template <typename T> int sgn(T val)
    {
        return (T(0) < val) - (val < T(0));
    }

    inline float degToRad(float x)
    {
        return x * 0.0174532925f;
    }

    inline float radToDeg(float x)
    {
        return x * 57.29577951f;
    }

    inline float clamp(float x, float lo, float hi)
    {
        return (x < lo) ? lo : ((x > hi) ? hi : x);
    }

    // Matrix3
    void addMatrix3(const float* m, float scalar, float* dst);

    void addMatrix3(const float* m1, const float* m2, float* dst);

    void subtractMatrix3(const float* m1, const float* m2, float* dst);

    void multiplyMatrix3(const float* m, float scalar, float* dst);

    void multiplyMatrix3(const float* m1, const float* m2, float* dst);

    void negateMatrix3(const float* m, float* dst);

    void transposeMatrix3(const float* m, float* dst);

    // Matrix4
    void addMatrix4(const float* m, float scalar, float* dst);

    void addMatrix4(const float* m1, const float* m2, float* dst);

    void subtractMatrix4(const float* m1, const float* m2, float* dst);

    void multiplyMatrix4(const float* m, float scalar, float* dst);

    void multiplyMatrix4(const float* m1, const float* m2, float* dst);

    void negateMatrix4(const float* m, float* dst);

    void transposeMatrix4(const float* m, float* dst);

    void transformVector3(const float* m, float x, float y, float z, float* dst);

    void transformVector3(const float* m, const float* v, float* dst);

    void transformVector4(const float* m, float x, float y, float z, float w, float* dst);

    void transformVector4(const float* m, const float* v, float* dst);

    void crossVector3(const float* v1, const float* v2, float* dst);

    bool linesIntersect(const Vector2& p0, const Vector2& p1,
                        const Vector2& p2, const Vector2& p3);
}
