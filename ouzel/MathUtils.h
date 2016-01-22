// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Vector2.h"

#define MATH_DEG_TO_RAD(x)          ((x) * 0.0174532925f)
#define MATH_RAD_TO_DEG(x)          ((x)* 57.29577951f)
#define MATH_RANDOM_MINUS1_1()      ((2.0f*((float)rand()/RAND_MAX))-1.0f)      // Returns a random float between -1 and 1.
#define MATH_RANDOM_0_1()           ((float)rand()/RAND_MAX)                    // Returns a random float between 0 and 1.
#define MATH_CLAMP(x, lo, hi)       ((x < lo) ? lo : ((x > hi) ? hi : x))

namespace ouzel
{
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
