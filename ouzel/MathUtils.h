//
//  MathUtils.h
//  Ouzel
//
//  Created by Elviss Strazdins on 11/07/15.
//  Copyright (c) 2015 Elviss. All rights reserved.
//

#pragma once

#define MATH_DEG_TO_RAD(x)          ((x) * 0.0174532925f)
#define MATH_RAD_TO_DEG(x)          ((x)* 57.29577951f)
#define MATH_RANDOM_MINUS1_1()      ((2.0f*((float)rand()/RAND_MAX))-1.0f)      // Returns a random float between -1 and 1.
#define MATH_RANDOM_0_1()           ((float)rand()/RAND_MAX)                    // Returns a random float between 0 and 1.
#define MATH_FLOAT_SMALL            1.0e-37f
#define MATH_TOLERANCE              2e-37f
#define MATH_E                      2.71828182845904523536f
#define MATH_LOG10E                 0.4342944819032518f
#define MATH_LOG2E                  1.442695040888963387f
#define MATH_PI                     3.14159265358979323846f
#define MATH_PIOVER2                1.57079632679489661923f
#define MATH_PIOVER4                0.785398163397448309616f
#define MATH_PIX2                   6.28318530717958647693f
#define MATH_EPSILON                0.000001f
#define MATH_CLAMP(x, lo, hi)       ((x < lo) ? lo : ((x > hi) ? hi : x))
#ifndef M_1_PI
#define M_1_PI                      0.31830988618379067154
#endif

namespace ouzel
{
    void smooth(float* x, float target, float elapsedTime, float responseTime);

    /**
     * Updates the given scalar towards the given target using a smoothing function.
     * The given rise and fall times determine the amount of smoothing (lag). Longer
     * rise and fall times yield a smoother result and more lag. To force the scalar to
     * follow the target closely, provide rise and fall times that are very small relative
     * to the given elapsed time.
     *
     * @param x the scalar to update.
     * @param target target value.
     * @param elapsedTime elapsed time between calls.
     * @param riseTime response time for rising slope (in the same units as elapsedTime).
     * @param fallTime response time for falling slope (in the same units as elapsedTime).
     */
    void smooth(float* x, float target, float elapsedTime, float riseTime, float fallTime);
    
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
}
