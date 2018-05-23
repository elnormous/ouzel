// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <cmath>
#include <cassert>
#include "Matrix4.hpp"
#include "Quaternion.hpp"
#include "MathUtils.hpp"

namespace ouzel
{
    const Matrix4 Matrix4::IDENTITY(1.0F, 0.0F, 0.0F, 0.0F,
                                    0.0F, 1.0F, 0.0F, 0.0F,
                                    0.0F, 0.0F, 1.0F, 0.0F,
                                    0.0F, 0.0F, 0.0F, 1.0F);

    const Matrix4 Matrix4::ZERO(0.0F, 0.0F, 0.0F, 0.0F,
                                0.0F, 0.0F, 0.0F, 0.0F,
                                0.0F, 0.0F, 0.0F, 0.0F,
                                0.0F, 0.0F, 0.0F, 0.0F);

    Matrix4::Matrix4(float pm11, float pm12, float pm13, float pm14,
                     float pm21, float pm22, float pm23, float pm24,
                     float pm31, float pm32, float pm33, float pm34,
                     float pm41, float pm42, float pm43, float pm44)
    {
        set(pm11, pm12, pm13, pm14,
            pm21, pm22, pm23, pm24,
            pm31, pm32, pm33, pm34,
            pm41, pm42, pm43, pm44);
    }

    Matrix4::Matrix4(const float* array)
    {
        set(array);
    }

    Matrix4::Matrix4(const Matrix4& copy)
    {
        std::copy(std::begin(copy.m), std::end(copy.m), m);
    }

    void Matrix4::createLookAt(const Vector3& eyePosition,
                               const Vector3& targetPosition,
                               const Vector3& up,
                               Matrix4& dst)
    {
        createLookAt(eyePosition.x, eyePosition.y, eyePosition.z,
                     targetPosition.x, targetPosition.y, targetPosition.z,
                     up.x, up.y, up.z, dst);
    }

    void Matrix4::createLookAt(float eyePositionX, float eyePositionY, float eyePositionZ,
                               float targetPositionX, float targetPositionY, float targetPositionZ,
                               float upX, float upY, float upZ,
                               Matrix4& dst)
    {
        Vector3 eye(eyePositionX, eyePositionY, eyePositionZ);
        Vector3 target(targetPositionX, targetPositionY, targetPositionZ);
        Vector3 up(upX, upY, upZ);
        up.normalize();

        Vector3 zaxis;
        Vector3::subtract(target, eye, zaxis);
        zaxis.normalize();

        Vector3 xaxis;
        Vector3::cross(up, zaxis, xaxis);
        xaxis.normalize();

        Vector3 yaxis;
        Vector3::cross(zaxis, xaxis, yaxis);
        yaxis.normalize();

        dst.m[0] = xaxis.x;
        dst.m[1] = yaxis.x;
        dst.m[2] = zaxis.x;
        dst.m[3] = 0.0F;

        dst.m[4] = xaxis.y;
        dst.m[5] = yaxis.y;
        dst.m[6] = zaxis.y;
        dst.m[7] = 0.0F;

        dst.m[8] = xaxis.z;
        dst.m[9] = yaxis.z;
        dst.m[10] = zaxis.z;
        dst.m[11] = 0.0F;

        dst.m[12] = Vector3::dot(xaxis, -eye);
        dst.m[13] = Vector3::dot(yaxis, -eye);
        dst.m[14] = Vector3::dot(zaxis, -eye);
        dst.m[15] = 1.0F;
    }

    void Matrix4::createPerspective(float fieldOfView, float aspectRatio,
                                    float zNearPlane, float zFarPlane,
                                    Matrix4& dst)
    {
        assert(zFarPlane != zNearPlane);

        float theta = fieldOfView * 0.5F;
        if (fabsf(fmodf(theta, PI / 2.0F)) < EPSILON)
        {
            // invalid field of view value
            return;
        }
        float divisor = tanf(theta);
        assert(divisor);
        float factor = 1.0F / divisor;

        dst.setZero();

        assert(aspectRatio);
        dst.m[0] = (1.0F / aspectRatio) * factor;
        dst.m[5] = factor;
        dst.m[10] = zFarPlane / (zFarPlane - zNearPlane);
        dst.m[11] = 1.0F;
        dst.m[14] = -zNearPlane * zFarPlane / (zFarPlane - zNearPlane);
    }

    void Matrix4::createOrthographicFromSize(float width, float height,
                                             float zNearPlane, float zFarPlane,
                                             Matrix4& dst)
    {
        float halfWidth = width / 2.0F;
        float halfHeight = height / 2.0F;
        createOrthographicOffCenter(-halfWidth, halfWidth,
                                    -halfHeight, halfHeight,
                                    zNearPlane, zFarPlane, dst);
    }

    void Matrix4::createOrthographicOffCenter(float left, float right, float bottom, float top,
                                              float zNearPlane, float zFarPlane, Matrix4& dst)
    {
        assert(right != left);
        assert(top != bottom);
        assert(zFarPlane != zNearPlane);

        dst.setZero();

        dst.m[0] = 2.0F / (right - left);
        dst.m[5] = 2.0F / (top - bottom);
        dst.m[10] = 1.0F / (zFarPlane - zNearPlane);
        dst.m[12] = (left + right) / (left - right);
        dst.m[13] = (bottom + top) / (bottom - top);
        dst.m[14] = zNearPlane / (zNearPlane - zFarPlane);
        dst.m[15] = 1.0F;
    }

    void Matrix4::createBillboard(const Vector3& objectPosition,
                                  const Vector3& cameraPosition,
                                  const Vector3& cameraUpVector,
                                  Matrix4& dst)
    {
        createBillboardHelper(objectPosition, cameraPosition, cameraUpVector, Vector3(), dst);
    }

    void Matrix4::createBillboard(const Vector3& objectPosition,
                                  const Vector3& cameraPosition,
                                  const Vector3& cameraUpVector,
                                  const Vector3& cameraForwardVector,
                                  Matrix4& dst)
    {
        createBillboardHelper(objectPosition, cameraPosition, cameraUpVector, cameraForwardVector, dst);
    }

    void Matrix4::createBillboardHelper(const Vector3& objectPosition,
                                        const Vector3& cameraPosition,
                                        const Vector3& cameraUpVector,
                                        const Vector3& cameraForwardVector,
                                        Matrix4& dst)
    {
        Vector3 delta = objectPosition - cameraPosition;
        bool isSufficientDelta = delta.lengthSquared() > EPSILON;

        dst.setIdentity();
        dst.m[3] = objectPosition.x;
        dst.m[7] = objectPosition.y;
        dst.m[11] = objectPosition.z;

        // As per the contracts for the 2 variants of createBillboard, we need
        // either a safe default or a sufficient distance between object and camera
        Vector3 target = isSufficientDelta ? cameraPosition : (objectPosition - cameraForwardVector);

        // A billboard is the inverse of a lookAt rotation
        Matrix4 lookAt;
        createLookAt(objectPosition, target, cameraUpVector, lookAt);
        dst.m[0] = lookAt.m[0];
        dst.m[1] = lookAt.m[4];
        dst.m[2] = lookAt.m[8];
        dst.m[4] = lookAt.m[1];
        dst.m[5] = lookAt.m[5];
        dst.m[6] = lookAt.m[9];
        dst.m[8] = lookAt.m[2];
        dst.m[9] = lookAt.m[6];
        dst.m[10] = lookAt.m[10];
    }

    void Matrix4::createScale(const Vector3& scale, Matrix4& dst)
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), dst.m);

        dst.m[0] = scale.x;
        dst.m[5] = scale.y;
        dst.m[10] = scale.z;
    }

    void Matrix4::createScale(float xScale, float yScale, float zScale, Matrix4& dst)
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), dst.m);

        dst.m[0] = xScale;
        dst.m[5] = yScale;
        dst.m[10] = zScale;
    }

    void Matrix4::createRotation(const Vector3& axis, float angle, Matrix4& dst)
    {
        float x = axis.x;
        float y = axis.y;
        float z = axis.z;

        // Make sure the input axis is normalized
        float n = x * x + y * y + z * z;
        if (n != 1.0F)
        {
            // Not normalized
            n = sqrtf(n);
            // Prevent divide too close to zero
            if (n >= EPSILON)
            {
                n = 1.0F / n;
                x *= n;
                y *= n;
                z *= n;
            }
        }

        float c = cosf(angle);
        float s = sinf(angle);

        float t = 1.0F - c;
        float tx = t * x;
        float ty = t * y;
        float tz = t * z;
        float txy = tx * y;
        float txz = tx * z;
        float tyz = ty * z;
        float sx = s * x;
        float sy = s * y;
        float sz = s * z;

        dst.m[0] = c + tx * x;
        dst.m[4] = txy - sz;
        dst.m[8] = txz + sy;
        dst.m[12] = 0.0F;

        dst.m[1] = txy + sz;
        dst.m[5] = c + ty * y;
        dst.m[9] = tyz - sx;
        dst.m[13] = 0.0F;

        dst.m[2] = txz - sy;
        dst.m[6] = tyz + sx;
        dst.m[10] = c + tz * z;
        dst.m[14] = 0.0F;

        dst.m[3] = 0.0F;
        dst.m[7] = 0.0F;
        dst.m[11] = 0.0F;
        dst.m[15] = 1.0F;
    }

    void Matrix4::createRotationX(float angle, Matrix4& dst)
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), dst.m);

        float c = cosf(angle);
        float s = sinf(angle);

        dst.m[5]  = c;
        dst.m[9]  = -s;
        dst.m[6]  = s;
        dst.m[10] = c;
    }

    void Matrix4::createRotationY(float angle, Matrix4& dst)
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), dst.m);

        float c = cosf(angle);
        float s = sinf(angle);

        dst.m[0]  = c;
        dst.m[8]  = s;
        dst.m[2]  = -s;
        dst.m[10] = c;
    }

    void Matrix4::createRotationZ(float angle, Matrix4& dst)
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), dst.m);

        float c = cosf(angle);
        float s = sinf(angle);

        dst.m[0] = c;
        dst.m[4] = -s;
        dst.m[1] = s;
        dst.m[5] = c;
    }

    void Matrix4::createTranslation(const Vector3& translation, Matrix4& dst)
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), dst.m);

        dst.m[12] = translation.x;
        dst.m[13] = translation.y;
        dst.m[14] = translation.z;
    }

    void Matrix4::createTranslation(float xTranslation, float yTranslation, float zTranslation, Matrix4& dst)
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), dst.m);

        dst.m[12] = xTranslation;
        dst.m[13] = yTranslation;
        dst.m[14] = zTranslation;
    }

    void Matrix4::add(float scalar)
    {
        add(scalar, *this);
    }

    void Matrix4::add(float scalar, Matrix4& dst)
    {
#if OUZEL_SUPPORTS_NEON
#if OUZEL_SUPPORTS_NEON_CHECK
        if (anrdoidNEONChecker.isNEONAvailable())
        {
#endif

#if OUZEL_64BITS // NEON64
        asm volatile
        (
            "ld4 {v0.4s, v1.4s, v2.4s, v3.4s}, [%1]   \n\t" // M[m0-m7] M[m8-m15]
            "ld1r {v4.4s}, [%2]                       \n\t" // ssss

            "fadd v8.4s, v0.4s, v4.4s                 \n\t" // DST->M[m0-m3] = M[m0-m3] + s
            "fadd v9.4s, v1.4s, v4.4s                 \n\t" // DST->M[m4-m7] = M[m4-m7] + s
            "fadd v10.4s, v2.4s, v4.4s                \n\t" // DST->M[m8-m11] = M[m8-m11] + s
            "fadd v11.4s, v3.4s, v4.4s                \n\t" // DST->M[m12-m15] = M[m12-m15] + s

            "st4 {v8.4s, v9.4s, v10.4s, v11.4s}, [%0] \n\t" // Result in V9
            :
            : "r"(dst.m), "r"(m), "r"(&scalar)
            : "v0", "v1", "v2", "v3", "v4", "v8", "v9", "v10", "v11", "memory"
        );
#else // NEON
        asm volatile
        (
            "vld1.32 {q0, q1}, [%1]!  \n\t" // M[m0-m7]
            "vld1.32 {q2, q3}, [%1]   \n\t" // M[m8-m15]
            "vld1.32 {d8[0]}, [%2]    \n\t" // s
            "vmov.f32 s17, s16        \n\t" // s
            "vmov.f32 s18, s16        \n\t" // s
            "vmov.f32 s19, s16        \n\t" // s

            "vadd.f32 q8, q0, q4      \n\t" // DST->M[m0-m3] = M[m0-m3] + s
            "vadd.f32 q9, q1, q4      \n\t" // DST->M[m4-m7] = M[m4-m7] + s
            "vadd.f32 q10, q2, q4     \n\t" // DST->M[m8-m11] = M[m8-m11] + s
            "vadd.f32 q11, q3, q4     \n\t" // DST->M[m12-m15] = M[m12-m15] + s

            "vst1.32 {q8, q9}, [%0]!  \n\t" // DST->M[m0-m7]
            "vst1.32 {q10, q11}, [%0] \n\t" // DST->M[m8-m15]
            :
            : "r"(dst.m), "r"(m), "r"(&scalar)
            : "q0", "q1", "q2", "q3", "q4", "q8", "q9", "q10", "q11", "memory"
        );
#endif

#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif

#elif OUZEL_SUPPORTS_SSE
        __m128 s = _mm_set1_ps(scalar);
        dst.col[0] = _mm_add_ps(col[0], s);
        dst.col[1] = _mm_add_ps(col[1], s);
        dst.col[2] = _mm_add_ps(col[2], s);
        dst.col[3] = _mm_add_ps(col[3], s);
#endif

#if (!OUZEL_SUPPORTS_NEON && !OUZEL_SUPPORTS_SSE) || OUZEL_SUPPORTS_NEON_CHECK
#if OUZEL_SUPPORTS_NEON_CHECK
        else
        {
#endif
        dst.m[0]  = m[0]  + scalar;
        dst.m[1]  = m[1]  + scalar;
        dst.m[2]  = m[2]  + scalar;
        dst.m[3]  = m[3]  + scalar;
        dst.m[4]  = m[4]  + scalar;
        dst.m[5]  = m[5]  + scalar;
        dst.m[6]  = m[6]  + scalar;
        dst.m[7]  = m[7]  + scalar;
        dst.m[8]  = m[8]  + scalar;
        dst.m[9]  = m[9]  + scalar;
        dst.m[10] = m[10] + scalar;
        dst.m[11] = m[11] + scalar;
        dst.m[12] = m[12] + scalar;
        dst.m[13] = m[13] + scalar;
        dst.m[14] = m[14] + scalar;
        dst.m[15] = m[15] + scalar;
#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif
#endif
    }

    void Matrix4::add(const Matrix4& matrix)
    {
        add(*this, matrix, *this);
    }

    void Matrix4::add(const Matrix4& m1, const Matrix4& m2, Matrix4& dst)
    {
#if OUZEL_SUPPORTS_NEON
#if OUZEL_SUPPORTS_NEON_CHECK
        if (anrdoidNEONChecker.isNEONAvailable())
        {
#endif

#if OUZEL_64BITS // NEON64
        asm volatile
        (
            "ld4 {v0.4s, v1.4s, v2.4s, v3.4s}, [%1]     \n\t" // M1[m0-m7] M1[m8-m15]
            "ld4 {v8.4s, v9.4s, v10.4s, v11.4s}, [%2]   \n\t" // M2[m0-m7] M2[m8-m15]

            "fadd v12.4s, v0.4s, v8.4s                  \n\t" // DST->M[m0-m3] = M1[m0-m3] + M2[m0-m3]
            "fadd v13.4s, v1.4s, v9.4s                  \n\t" // DST->M[m4-m7] = M1[m4-m7] + M2[m4-m7]
            "fadd v14.4s, v2.4s, v10.4s                 \n\t" // DST->M[m8-m11] = M1[m8-m11] + M2[m8-m11]
            "fadd v15.4s, v3.4s, v11.4s                 \n\t" // DST->M[m12-m15] = M1[m12-m15] + M2[m12-m15]

            "st4 {v12.4s, v13.4s, v14.4s, v15.4s}, [%0] \n\t" // DST->M[m0-m7] DST->M[m8-m15]
            :
            : "r"(dst.m), "r"(m1.m), "r"(m2.m)
            : "v0", "v1", "v2", "v3", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "memory"
        );
#else // NEON
        asm volatile
        (
            "vld1.32 {q0, q1}, [%1]!    \n\t" // M1[m0-m7]
            "vld1.32 {q2, q3}, [%1]     \n\t" // M1[m8-m15]
            "vld1.32 {q8, q9}, [%2]!    \n\t" // M2[m0-m7]
            "vld1.32 {q10, q11}, [%2]   \n\t" // M2[m8-m15]

            "vadd.f32 q12, q0, q8       \n\t" // DST->M[m0-m3] = M1[m0-m3] + M2[m0-m3]
            "vadd.f32 q13, q1, q9       \n\t" // DST->M[m4-m7] = M1[m4-m7] + M2[m4-m7]
            "vadd.f32 q14, q2, q10      \n\t" // DST->M[m8-m11] = M1[m8-m11] + M2[m8-m11]
            "vadd.f32 q15, q3, q11      \n\t" // DST->M[m12-m15] = M1[m12-m15] + M2[m12-m15]

            "vst1.32 {q12, q13}, [%0]!  \n\t" // DST->M[m0-m7]
            "vst1.32 {q14, q15}, [%0]   \n\t" // DST->M[m8-m15]
            :
            : "r"(dst.m), "r"(m1.m), "r"(m2.m)
            : "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "memory"
        );
#endif

#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif

#elif OUZEL_SUPPORTS_SSE
        dst.col[0] = _mm_add_ps(m1.col[0], m2.col[0]);
        dst.col[1] = _mm_add_ps(m1.col[1], m2.col[1]);
        dst.col[2] = _mm_add_ps(m1.col[2], m2.col[2]);
        dst.col[3] = _mm_add_ps(m1.col[3], m2.col[3]);
#endif

#if (!OUZEL_SUPPORTS_NEON && !OUZEL_SUPPORTS_SSE) || OUZEL_SUPPORTS_NEON_CHECK
#if OUZEL_SUPPORTS_NEON_CHECK
        else
        {
#endif
        dst.m[0]  = m1.m[0]  + m2.m[0];
        dst.m[1]  = m1.m[1]  + m2.m[1];
        dst.m[2]  = m1.m[2]  + m2.m[2];
        dst.m[3]  = m1.m[3]  + m2.m[3];
        dst.m[4]  = m1.m[4]  + m2.m[4];
        dst.m[5]  = m1.m[5]  + m2.m[5];
        dst.m[6]  = m1.m[6]  + m2.m[6];
        dst.m[7]  = m1.m[7]  + m2.m[7];
        dst.m[8]  = m1.m[8]  + m2.m[8];
        dst.m[9]  = m1.m[9]  + m2.m[9];
        dst.m[10] = m1.m[10] + m2.m[10];
        dst.m[11] = m1.m[11] + m2.m[11];
        dst.m[12] = m1.m[12] + m2.m[12];
        dst.m[13] = m1.m[13] + m2.m[13];
        dst.m[14] = m1.m[14] + m2.m[14];
        dst.m[15] = m1.m[15] + m2.m[15];
#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif
#endif
    }

    float Matrix4::determinant() const
    {
        float a0 = m[0] * m[5] - m[1] * m[4];
        float a1 = m[0] * m[6] - m[2] * m[4];
        float a2 = m[0] * m[7] - m[3] * m[4];
        float a3 = m[1] * m[6] - m[2] * m[5];
        float a4 = m[1] * m[7] - m[3] * m[5];
        float a5 = m[2] * m[7] - m[3] * m[6];
        float b0 = m[8] * m[13] - m[9] * m[12];
        float b1 = m[8] * m[14] - m[10] * m[12];
        float b2 = m[8] * m[15] - m[11] * m[12];
        float b3 = m[9] * m[14] - m[10] * m[13];
        float b4 = m[9] * m[15] - m[11] * m[13];
        float b5 = m[10] * m[15] - m[11] * m[14];

        // Calculate the determinant
        return a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
    }

    void Matrix4::getUpVector(Vector3& dst) const
    {
        dst.x = m[4];
        dst.y = m[5];
        dst.z = m[6];
    }

    void Matrix4::getDownVector(Vector3& dst) const
    {
        dst.x = -m[4];
        dst.y = -m[5];
        dst.z = -m[6];
    }

    void Matrix4::getLeftVector(Vector3& dst) const
    {
        dst.x = -m[0];
        dst.y = -m[1];
        dst.z = -m[2];
    }

    void Matrix4::getRightVector(Vector3& dst) const
    {
        dst.x = m[0];
        dst.y = m[1];
        dst.z = m[2];
    }

    void Matrix4::getForwardVector(Vector3& dst) const
    {
        dst.x = -m[8];
        dst.y = -m[9];
        dst.z = -m[10];
    }

    void Matrix4::getBackVector(Vector3& dst) const
    {
        dst.x = m[8];
        dst.y = m[9];
        dst.z = m[10];
    }

    bool Matrix4::invert()
    {
        return invert(*this);
    }

    bool Matrix4::invert(Matrix4& dst) const
    {
        float a0 = m[0] * m[5] - m[1] * m[4];
        float a1 = m[0] * m[6] - m[2] * m[4];
        float a2 = m[0] * m[7] - m[3] * m[4];
        float a3 = m[1] * m[6] - m[2] * m[5];
        float a4 = m[1] * m[7] - m[3] * m[5];
        float a5 = m[2] * m[7] - m[3] * m[6];
        float b0 = m[8] * m[13] - m[9] * m[12];
        float b1 = m[8] * m[14] - m[10] * m[12];
        float b2 = m[8] * m[15] - m[11] * m[12];
        float b3 = m[9] * m[14] - m[10] * m[13];
        float b4 = m[9] * m[15] - m[11] * m[13];
        float b5 = m[10] * m[15] - m[11] * m[14];

        // Calculate the determinant
        float det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

        // Close to zero, can't invert
        if (fabs(det) < EPSILON)
            return false;

        Matrix4 inverse;
        inverse.m[0]  = m[5] * b5 - m[6] * b4 + m[7] * b3;
        inverse.m[1]  = -m[1] * b5 + m[2] * b4 - m[3] * b3;
        inverse.m[2]  = m[13] * a5 - m[14] * a4 + m[15] * a3;
        inverse.m[3]  = -m[9] * a5 + m[10] * a4 - m[11] * a3;

        inverse.m[4]  = -m[4] * b5 + m[6] * b2 - m[7] * b1;
        inverse.m[5]  = m[0] * b5 - m[2] * b2 + m[3] * b1;
        inverse.m[6]  = -m[12] * a5 + m[14] * a2 - m[15] * a1;
        inverse.m[7]  = m[8] * a5 - m[10] * a2 + m[11] * a1;

        inverse.m[8]  = m[4] * b4 - m[5] * b2 + m[7] * b0;
        inverse.m[9]  = -m[0] * b4 + m[1] * b2 - m[3] * b0;
        inverse.m[10] = m[12] * a4 - m[13] * a2 + m[15] * a0;
        inverse.m[11] = -m[8] * a4 + m[9] * a2 - m[11] * a0;

        inverse.m[12] = -m[4] * b3 + m[5] * b1 - m[6] * b0;
        inverse.m[13] = m[0] * b3 - m[1] * b1 + m[2] * b0;
        inverse.m[14] = -m[12] * a3 + m[13] * a1 - m[14] * a0;
        inverse.m[15] = m[8] * a3 - m[9] * a1 + m[10] * a0;

        multiply(inverse, 1.0F / det, dst);

        return true;
    }

    bool Matrix4::isIdentity() const
    {
        return std::equal(m, m + sizeof(m) / sizeof(float), IDENTITY.m);
    }

    void Matrix4::multiply(float scalar)
    {
        multiply(scalar, *this);
    }

    void Matrix4::multiply(float scalar, Matrix4& dst) const
    {
        multiply(*this, scalar, dst);
    }

    void Matrix4::multiply(const Matrix4& m, float scalar, Matrix4& dst)
    {
#if OUZEL_SUPPORTS_NEON
#if OUZEL_SUPPORTS_NEON_CHECK
        if (anrdoidNEONChecker.isNEONAvailable())
        {
#endif

#if OUZEL_64BITS // NEON64
        asm volatile
        (
            "ld1 {v0.s}[0], [%2]                      \n\t" // s
            "ld4 {v4.4s, v5.4s, v6.4s, v7.4s}, [%1]   \n\t" // M[m0-m7] M[m8-m15]

            "fmul v8.4s, v4.4s, v0.s[0]               \n\t" // DST->M[m0-m3] = M[m0-m3] * s
            "fmul v9.4s, v5.4s, v0.s[0]               \n\t" // DST->M[m4-m7] = M[m4-m7] * s
            "fmul v10.4s, v6.4s, v0.s[0]              \n\t" // DST->M[m8-m11] = M[m8-m11] * s
            "fmul v11.4s, v7.4s, v0.s[0]              \n\t" // DST->M[m12-m15] = M[m12-m15] * s

            "st4 {v8.4s, v9.4s, v10.4s, v11.4s}, [%0] \n\t" // DST->M[m0-m7] DST->M[m8-m15]
            :
            : "r"(dst.m), "r"(m.m), "r"(&scalar)
            : "v0", "v4", "v5", "v6", "v7", "v8", "v9", "v10", "v11", "memory"
        );
#else // NEON
        asm volatile
        (
            "vld1.32 {d0[0]}, [%2]   \n\t" // M[m0-m7]
            "vld1.32 {q4-q5}, [%1]!  \n\t" // M[m8-m15]
            "vld1.32 {q6-q7}, [%1]   \n\t" // s

            "vmul.f32 q8, q4, d0[0]  \n\t" // DST->M[m0-m3] = M[m0-m3] * s
            "vmul.f32 q9, q5, d0[0]  \n\t" // DST->M[m4-m7] = M[m4-m7] * s
            "vmul.f32 q10, q6, d0[0] \n\t" // DST->M[m8-m11] = M[m8-m11] * s
            "vmul.f32 q11, q7, d0[0] \n\t" // DST->M[m12-m15] = M[m12-m15] * s

            "vst1.32 {q8-q9}, [%0]!  \n\t" // DST->M[m0-m7]
            "vst1.32 {q10-q11}, [%0] \n\t" // DST->M[m8-m15]
            :
            : "r"(dst.m), "r"(m.m), "r"(&scalar)
            : "q0", "q4", "q5", "q6", "q7", "q8", "q9", "q10", "q11", "memory"
        );
#endif

#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif

#elif OUZEL_SUPPORTS_SSE
        __m128 s = _mm_set1_ps(scalar);
        dst.col[0] = _mm_mul_ps(m.col[0], s);
        dst.col[1] = _mm_mul_ps(m.col[1], s);
        dst.col[2] = _mm_mul_ps(m.col[2], s);
        dst.col[3] = _mm_mul_ps(m.col[3], s);
#endif

#if (!OUZEL_SUPPORTS_NEON && !OUZEL_SUPPORTS_SSE) || OUZEL_SUPPORTS_NEON_CHECK
#if OUZEL_SUPPORTS_NEON_CHECK
        else
        {
#endif
        dst.m[0]  = m.m[0]  * scalar;
        dst.m[1]  = m.m[1]  * scalar;
        dst.m[2]  = m.m[2]  * scalar;
        dst.m[3]  = m.m[3]  * scalar;
        dst.m[4]  = m.m[4]  * scalar;
        dst.m[5]  = m.m[5]  * scalar;
        dst.m[6]  = m.m[6]  * scalar;
        dst.m[7]  = m.m[7]  * scalar;
        dst.m[8]  = m.m[8]  * scalar;
        dst.m[9]  = m.m[9]  * scalar;
        dst.m[10] = m.m[10] * scalar;
        dst.m[11] = m.m[11] * scalar;
        dst.m[12] = m.m[12] * scalar;
        dst.m[13] = m.m[13] * scalar;
        dst.m[14] = m.m[14] * scalar;
        dst.m[15] = m.m[15] * scalar;
#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif
#endif
    }

    void Matrix4::multiply(const Matrix4& matrix)
    {
        multiply(*this, matrix, *this);
    }

    void Matrix4::multiply(const Matrix4& m1, const Matrix4& m2, Matrix4& dst)
    {
#if OUZEL_SUPPORTS_NEON
#if OUZEL_SUPPORTS_NEON_CHECK
        if (anrdoidNEONChecker.isNEONAvailable())
        {
#endif

#if OUZEL_64BITS // NEON64
        asm volatile
        (
            "ld1 {v8.4s, v9.4s, v10.4s, v11.4s}, [%1]   \n\t" // M1[m0-m7] M1[m8-m15] M2[m0-m7]  M2[m8-m15]
            "ld4 {v0.4s, v1.4s, v2.4s, v3.4s}, [%2]     \n\t" // M2[m0-m15]

            "fmul v12.4s, v8.4s, v0.s[0]                \n\t" // DST->M[m0-m3] = M1[m0-m3] * M2[m0]
            "fmul v13.4s, v8.4s, v0.s[1]                \n\t" // DST->M[m4-m7] = M1[m4-m7] * M2[m4]
            "fmul v14.4s, v8.4s, v0.s[2]                \n\t" // DST->M[m8-m11] = M1[m8-m11] * M2[m8]
            "fmul v15.4s, v8.4s, v0.s[3]                \n\t" // DST->M[m12-m15] = M1[m12-m15] * M2[m12]

            "fmla v12.4s, v9.4s, v1.s[0]                \n\t" // DST->M[m0-m3] += M1[m0-m3] * M2[m1]
            "fmla v13.4s, v9.4s, v1.s[1]                \n\t" // DST->M[m4-m7] += M1[m4-m7] * M2[m5]
            "fmla v14.4s, v9.4s, v1.s[2]                \n\t" // DST->M[m8-m11] += M1[m8-m11] * M2[m9]
            "fmla v15.4s, v9.4s, v1.s[3]                \n\t" // DST->M[m12-m15] += M1[m12-m15] * M2[m13]

            "fmla v12.4s, v10.4s, v2.s[0]               \n\t" // DST->M[m0-m3] += M1[m0-m3] * M2[m2]
            "fmla v13.4s, v10.4s, v2.s[1]               \n\t" // DST->M[m4-m7] += M1[m4-m7] * M2[m6]
            "fmla v14.4s, v10.4s, v2.s[2]               \n\t" // DST->M[m8-m11] += M1[m8-m11] * M2[m10]
            "fmla v15.4s, v10.4s, v2.s[3]               \n\t" // DST->M[m12-m15] += M1[m12-m15] * M2[m14]

            "fmla v12.4s, v11.4s, v3.s[0]               \n\t" // DST->M[m0-m3] += M1[m0-m3] * M2[m3]
            "fmla v13.4s, v11.4s, v3.s[1]               \n\t" // DST->M[m4-m7] += M1[m4-m7] * M2[m7]
            "fmla v14.4s, v11.4s, v3.s[2]               \n\t" // DST->M[m8-m11] += M1[m8-m11] * M2[m11]
            "fmla v15.4s, v11.4s, v3.s[3]               \n\t" // DST->M[m12-m15] += M1[m12-m15] * M2[m15]

            "st1 {v12.4s, v13.4s, v14.4s, v15.4s}, [%0] \n\t" // DST->M[m0-m7]// DST->M[m8-m15]

            : // output
            : "r"(dst.m), "r"(m1.m), "r"(m2.m) // input - note *value* of pointer doesn't change
            : "memory", "v0", "v1", "v2", "v3", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
        );
#else // NEON
        asm volatile
        (
            "vld1.32 {d16 - d19}, [%1]! \n\t" // M1[m0-m7]
            "vld1.32 {d20 - d23}, [%1]  \n\t" // M1[m8-m15]
            "vld1.32 {d0 - d3}, [%2]!   \n\t" // M2[m0-m7]
            "vld1.32 {d4 - d7}, [%2]    \n\t" // M2[m8-m15]

            "vmul.f32 q12, q8, d0[0]    \n\t" // DST->M[m0-m3] = M1[m0-m3] * M2[m0]
            "vmul.f32 q13, q8, d2[0]    \n\t" // DST->M[m4-m7] = M1[m4-m7] * M2[m4]
            "vmul.f32 q14, q8, d4[0]    \n\t" // DST->M[m8-m11] = M1[m8-m11] * M2[m8]
            "vmul.f32 q15, q8, d6[0]    \n\t" // DST->M[m12-m15] = M1[m12-m15] * M2[m12]

            "vmla.f32 q12, q9, d0[1]    \n\t" // DST->M[m0-m3] += M1[m0-m3] * M2[m1]
            "vmla.f32 q13, q9, d2[1]    \n\t" // DST->M[m4-m7] += M1[m4-m7] * M2[m5]
            "vmla.f32 q14, q9, d4[1]    \n\t" // DST->M[m8-m11] += M1[m8-m11] * M2[m9]
            "vmla.f32 q15, q9, d6[1]    \n\t" // DST->M[m12-m15] += M1[m12-m15] * M2[m13]

            "vmla.f32 q12, q10, d1[0]   \n\t" // DST->M[m0-m3] += M1[m0-m3] * M2[m2]
            "vmla.f32 q13, q10, d3[0]   \n\t" // DST->M[m4-m7] += M1[m4-m7] * M2[m6]
            "vmla.f32 q14, q10, d5[0]   \n\t" // DST->M[m8-m11] += M1[m8-m11] * M2[m10]
            "vmla.f32 q15, q10, d7[0]   \n\t" // DST->M[m12-m15] += M1[m12-m15] * M2[m14]

            "vmla.f32 q12, q11, d1[1]   \n\t" // DST->M[m0-m3] += M1[m0-m3] * M2[m3]
            "vmla.f32 q13, q11, d3[1]   \n\t" // DST->M[m4-m7] += M1[m4-m7] * M2[m7]
            "vmla.f32 q14, q11, d5[1]   \n\t" // DST->M[m8-m11] += M1[m8-m11] * M2[m11]
            "vmla.f32 q15, q11, d7[1]   \n\t" // DST->M[m12-m15] += M1[m12-m15] * M2[m15]

            "vst1.32 {d24 - d27}, [%0]! \n\t" // DST->M[m0-m7]
            "vst1.32 {d28 - d31}, [%0]  \n\t" // DST->M[m8-m15]

            : // output
            : "r"(dst.m), "r"(m1.m), "r"(m2.m) // input - note *value* of pointer doesn't change
            : "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
        );
#endif

#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif

#elif OUZEL_SUPPORTS_SSE
        __m128 dest[4];
        {
            __m128 e0 = _mm_shuffle_ps(m2.col[0], m2.col[0], _MM_SHUFFLE(0, 0, 0, 0));
            __m128 e1 = _mm_shuffle_ps(m2.col[0], m2.col[0], _MM_SHUFFLE(1, 1, 1, 1));
            __m128 e2 = _mm_shuffle_ps(m2.col[0], m2.col[0], _MM_SHUFFLE(2, 2, 2, 2));
            __m128 e3 = _mm_shuffle_ps(m2.col[0], m2.col[0], _MM_SHUFFLE(3, 3, 3, 3));

            __m128 v0 = _mm_mul_ps(m1.col[0], e0);
            __m128 v1 = _mm_mul_ps(m1.col[1], e1);
            __m128 v2 = _mm_mul_ps(m1.col[2], e2);
            __m128 v3 = _mm_mul_ps(m1.col[3], e3);

            __m128 a0 = _mm_add_ps(v0, v1);
            __m128 a1 = _mm_add_ps(v2, v3);
            __m128 a2 = _mm_add_ps(a0, a1);

            dest[0] = a2;
        }

        {
            __m128 e0 = _mm_shuffle_ps(m2.col[1], m2.col[1], _MM_SHUFFLE(0, 0, 0, 0));
            __m128 e1 = _mm_shuffle_ps(m2.col[1], m2.col[1], _MM_SHUFFLE(1, 1, 1, 1));
            __m128 e2 = _mm_shuffle_ps(m2.col[1], m2.col[1], _MM_SHUFFLE(2, 2, 2, 2));
            __m128 e3 = _mm_shuffle_ps(m2.col[1], m2.col[1], _MM_SHUFFLE(3, 3, 3, 3));

            __m128 v0 = _mm_mul_ps(m1.col[0], e0);
            __m128 v1 = _mm_mul_ps(m1.col[1], e1);
            __m128 v2 = _mm_mul_ps(m1.col[2], e2);
            __m128 v3 = _mm_mul_ps(m1.col[3], e3);

            __m128 a0 = _mm_add_ps(v0, v1);
            __m128 a1 = _mm_add_ps(v2, v3);
            __m128 a2 = _mm_add_ps(a0, a1);

            dest[1] = a2;
        }

        {
            __m128 e0 = _mm_shuffle_ps(m2.col[2], m2.col[2], _MM_SHUFFLE(0, 0, 0, 0));
            __m128 e1 = _mm_shuffle_ps(m2.col[2], m2.col[2], _MM_SHUFFLE(1, 1, 1, 1));
            __m128 e2 = _mm_shuffle_ps(m2.col[2], m2.col[2], _MM_SHUFFLE(2, 2, 2, 2));
            __m128 e3 = _mm_shuffle_ps(m2.col[2], m2.col[2], _MM_SHUFFLE(3, 3, 3, 3));

            __m128 v0 = _mm_mul_ps(m1.col[0], e0);
            __m128 v1 = _mm_mul_ps(m1.col[1], e1);
            __m128 v2 = _mm_mul_ps(m1.col[2], e2);
            __m128 v3 = _mm_mul_ps(m1.col[3], e3);

            __m128 a0 = _mm_add_ps(v0, v1);
            __m128 a1 = _mm_add_ps(v2, v3);
            __m128 a2 = _mm_add_ps(a0, a1);

            dest[2] = a2;
        }

        {
            __m128 e0 = _mm_shuffle_ps(m2.col[3], m2.col[3], _MM_SHUFFLE(0, 0, 0, 0));
            __m128 e1 = _mm_shuffle_ps(m2.col[3], m2.col[3], _MM_SHUFFLE(1, 1, 1, 1));
            __m128 e2 = _mm_shuffle_ps(m2.col[3], m2.col[3], _MM_SHUFFLE(2, 2, 2, 2));
            __m128 e3 = _mm_shuffle_ps(m2.col[3], m2.col[3], _MM_SHUFFLE(3, 3, 3, 3));

            __m128 v0 = _mm_mul_ps(m1.col[0], e0);
            __m128 v1 = _mm_mul_ps(m1.col[1], e1);
            __m128 v2 = _mm_mul_ps(m1.col[2], e2);
            __m128 v3 = _mm_mul_ps(m1.col[3], e3);

            __m128 a0 = _mm_add_ps(v0, v1);
            __m128 a1 = _mm_add_ps(v2, v3);
            __m128 a2 = _mm_add_ps(a0, a1);

            dest[3] = a2;
        }
        dst.col[0] = dest[0];
        dst.col[1] = dest[1];
        dst.col[2] = dest[2];
        dst.col[3] = dest[3];
#endif

#if (!OUZEL_SUPPORTS_NEON && !OUZEL_SUPPORTS_SSE) || OUZEL_SUPPORTS_NEON_CHECK
#if OUZEL_SUPPORTS_NEON_CHECK
        else
        {
#endif
        float product[16];

        product[0]  = m1.m[0] * m2.m[0]  + m1.m[4] * m2.m[1] + m1.m[8]   * m2.m[2]  + m1.m[12] * m2.m[3];
        product[1]  = m1.m[1] * m2.m[0]  + m1.m[5] * m2.m[1] + m1.m[9]   * m2.m[2]  + m1.m[13] * m2.m[3];
        product[2]  = m1.m[2] * m2.m[0]  + m1.m[6] * m2.m[1] + m1.m[10]  * m2.m[2]  + m1.m[14] * m2.m[3];
        product[3]  = m1.m[3] * m2.m[0]  + m1.m[7] * m2.m[1] + m1.m[11]  * m2.m[2]  + m1.m[15] * m2.m[3];

        product[4]  = m1.m[0] * m2.m[4]  + m1.m[4] * m2.m[5] + m1.m[8]   * m2.m[6]  + m1.m[12] * m2.m[7];
        product[5]  = m1.m[1] * m2.m[4]  + m1.m[5] * m2.m[5] + m1.m[9]   * m2.m[6]  + m1.m[13] * m2.m[7];
        product[6]  = m1.m[2] * m2.m[4]  + m1.m[6] * m2.m[5] + m1.m[10]  * m2.m[6]  + m1.m[14] * m2.m[7];
        product[7]  = m1.m[3] * m2.m[4]  + m1.m[7] * m2.m[5] + m1.m[11]  * m2.m[6]  + m1.m[15] * m2.m[7];

        product[8]  = m1.m[0] * m2.m[8]  + m1.m[4] * m2.m[9] + m1.m[8]   * m2.m[10] + m1.m[12] * m2.m[11];
        product[9]  = m1.m[1] * m2.m[8]  + m1.m[5] * m2.m[9] + m1.m[9]   * m2.m[10] + m1.m[13] * m2.m[11];
        product[10] = m1.m[2] * m2.m[8]  + m1.m[6] * m2.m[9] + m1.m[10]  * m2.m[10] + m1.m[14] * m2.m[11];
        product[11] = m1.m[3] * m2.m[8]  + m1.m[7] * m2.m[9] + m1.m[11]  * m2.m[10] + m1.m[15] * m2.m[11];

        product[12] = m1.m[0] * m2.m[12] + m1.m[4] * m2.m[13] + m1.m[8]  * m2.m[14] + m1.m[12] * m2.m[15];
        product[13] = m1.m[1] * m2.m[12] + m1.m[5] * m2.m[13] + m1.m[9]  * m2.m[14] + m1.m[13] * m2.m[15];
        product[14] = m1.m[2] * m2.m[12] + m1.m[6] * m2.m[13] + m1.m[10] * m2.m[14] + m1.m[14] * m2.m[15];
        product[15] = m1.m[3] * m2.m[12] + m1.m[7] * m2.m[13] + m1.m[11] * m2.m[14] + m1.m[15] * m2.m[15];

        std::copy(std::begin(product), std::end(product), dst.m);
#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif
#endif
    }

    void Matrix4::negate()
    {
        negate(*this);
    }

    void Matrix4::negate(Matrix4& dst) const
    {
#if OUZEL_SUPPORTS_NEON
#if OUZEL_SUPPORTS_NEON_CHECK
        if (anrdoidNEONChecker.isNEONAvailable())
        {
#endif

#if OUZEL_64BITS // NEON64
        asm volatile
        (
            "ld4 {v0.4s, v1.4s, v2.4s, v3.4s}, [%1] \n\t" // load m0-m7 load m8-m15

            "fneg v4.4s, v0.4s                      \n\t" // negate m0-m3
            "fneg v5.4s, v1.4s                      \n\t" // negate m4-m7
            "fneg v6.4s, v2.4s                      \n\t" // negate m8-m15
            "fneg v7.4s, v3.4s                      \n\t" // negate m8-m15

            "st4 {v4.4s, v5.4s, v6.4s, v7.4s}, [%0] \n\t" // store m0-m7 store m8-m15
            :
            : "r"(dst.m), "r"(m)
            : "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "memory"
        );
#else // NEON
        asm volatile
        (
            "vld1.32 {q0-q1}, [%1]! \n\t" // load m0-m7
            "vld1.32 {q2-q3}, [%1]  \n\t" // load m8-m15

            "vneg.f32 q4, q0        \n\t" // negate m0-m3
            "vneg.f32 q5, q1        \n\t" // negate m4-m7
            "vneg.f32 q6, q2        \n\t" // negate m8-m15
            "vneg.f32 q7, q3        \n\t" // negate m8-m15

            "vst1.32 {q4-q5}, [%0]! \n\t" // store m0-m7
            "vst1.32 {q6-q7}, [%0]  \n\t" // store m8-m15
            :
            : "r"(dst.m), "r"(m)
            : "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "memory"
        );
#endif

#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif

#elif OUZEL_SUPPORTS_SSE
        __m128 z = _mm_setzero_ps();
        dst.col[0] = _mm_sub_ps(z, col[0]);
        dst.col[1] = _mm_sub_ps(z, col[1]);
        dst.col[2] = _mm_sub_ps(z, col[2]);
        dst.col[3] = _mm_sub_ps(z, col[3]);
#endif

#if (!OUZEL_SUPPORTS_NEON && !OUZEL_SUPPORTS_SSE) || OUZEL_SUPPORTS_NEON_CHECK
#if OUZEL_SUPPORTS_NEON_CHECK
        else
        {
#endif
        dst.m[0]  = -m[0];
        dst.m[1]  = -m[1];
        dst.m[2]  = -m[2];
        dst.m[3]  = -m[3];
        dst.m[4]  = -m[4];
        dst.m[5]  = -m[5];
        dst.m[6]  = -m[6];
        dst.m[7]  = -m[7];
        dst.m[8]  = -m[8];
        dst.m[9]  = -m[9];
        dst.m[10] = -m[10];
        dst.m[11] = -m[11];
        dst.m[12] = -m[12];
        dst.m[13] = -m[13];
        dst.m[14] = -m[14];
        dst.m[15] = -m[15];
#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif
#endif
    }

    void Matrix4::rotate(const Vector3& axis, float angle)
    {
        rotate(axis, angle, *this);
    }

    void Matrix4::rotate(const Vector3& axis, float angle, Matrix4& dst) const
    {
        Matrix4 r;
        createRotation(axis, angle, r);
        multiply(*this, r, dst);
    }

    void Matrix4::rotateX(float angle)
    {
        rotateX(angle, *this);
    }

    void Matrix4::rotateX(float angle, Matrix4& dst) const
    {
        Matrix4 r;
        createRotationX(angle, r);
        multiply(*this, r, dst);
    }

    void Matrix4::rotateY(float angle)
    {
        rotateY(angle, *this);
    }

    void Matrix4::rotateY(float angle, Matrix4& dst) const
    {
        Matrix4 r;
        createRotationY(angle, r);
        multiply(*this, r, dst);
    }

    void Matrix4::rotateZ(float angle)
    {
        rotateZ(angle, *this);
    }

    void Matrix4::rotateZ(float angle, Matrix4& dst) const
    {
        Matrix4 r;
        createRotationZ(angle, r);
        multiply(*this, r, dst);
    }

    void Matrix4::scale(float value)
    {
        scale(value, *this);
    }

    void Matrix4::scale(float value, Matrix4& dst) const
    {
        scale(value, value, value, dst);
    }

    void Matrix4::scale(float xScale, float yScale, float zScale)
    {
        scale(xScale, yScale, zScale, *this);
    }

    void Matrix4::scale(float xScale, float yScale, float zScale, Matrix4& dst) const
    {
        Matrix4 s;
        createScale(xScale, yScale, zScale, s);
        multiply(*this, s, dst);
    }

    void Matrix4::scale(const Vector3& s)
    {
        scale(s.x, s.y, s.z, *this);
    }

    void Matrix4::scale(const Vector3& s, Matrix4& dst) const
    {
        scale(s.x, s.y, s.z, dst);
    }

    void Matrix4::set(float m11, float m12, float m13, float m14,
                      float m21, float m22, float m23, float m24,
                      float m31, float m32, float m33, float m34,
                      float m41, float m42, float m43, float m44)
    {
        m[0]  = m11;
        m[1]  = m21;
        m[2]  = m31;
        m[3]  = m41;
        m[4]  = m12;
        m[5]  = m22;
        m[6]  = m32;
        m[7]  = m42;
        m[8]  = m13;
        m[9]  = m23;
        m[10] = m33;
        m[11] = m43;
        m[12] = m14;
        m[13] = m24;
        m[14] = m34;
        m[15] = m44;
    }

    void Matrix4::set(const float* array)
    {
        assert(array);
        std::copy(array, array + sizeof(m) / sizeof(float), m);
    }

    void Matrix4::setIdentity()
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), m);
    }

    void Matrix4::setZero()
    {
        std::fill(m, m + sizeof(m) / sizeof(float), 0.0F);
    }

    void Matrix4::subtract(const Matrix4& matrix)
    {
        subtract(*this, matrix, *this);
    }

    void Matrix4::subtract(const Matrix4& m1, const Matrix4& m2, Matrix4& dst)
    {
#if OUZEL_SUPPORTS_NEON
#if OUZEL_SUPPORTS_NEON_CHECK
        if (anrdoidNEONChecker.isNEONAvailable())
        {
#endif

#if OUZEL_64BITS // NEON64
        asm volatile
        (
            "ld4 {v0.4s, v1.4s, v2.4s, v3.4s}, [%1]     \n\t" // M1[m0-m7] M1[m8-m15]
            "ld4 {v8.4s, v9.4s, v10.4s, v11.4s}, [%2]   \n\t" // M2[m0-m7] M2[m8-m15]

            "fsub v12.4s, v0.4s, v8.4s                  \n\t" // DST->M[m0-m3] = M1[m0-m3] - M2[m0-m3]
            "fsub v13.4s, v1.4s, v9.4s                  \n\t" // DST->M[m4-m7] = M1[m4-m7] - M2[m4-m7]
            "fsub v14.4s, v2.4s, v10.4s                 \n\t" // DST->M[m8-m11] = M1[m8-m11] - M2[m8-m11]
            "fsub v15.4s, v3.4s, v11.4s                 \n\t" // DST->M[m12-m15] = M1[m12-m15] - M2[m12-m15]

            "st4 {v12.4s, v13.4s, v14.4s, v15.4s}, [%0] \n\t" // DST->M[m0-m7] DST->M[m8-m15]
            :
            : "r"(dst.m), "r"(m1.m), "r"(m2.m)
            : "v0", "v1", "v2", "v3", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "memory"
        );
#else // NEON
        asm volatile
        (
            "vld1.32 {q0, q1}, [%1]!   \n\t" // M1[m0-m7]
            "vld1.32 {q2, q3}, [%1]    \n\t" // M1[m8-m15]
            "vld1.32 {q8, q9}, [%2]!   \n\t" // M2[m0-m7]
            "vld1.32 {q10, q11}, [%2]  \n\t" // M2[m8-m15]

            "vsub.f32 q12, q0, q8      \n\t" // DST->M[m0-m3] = M1[m0-m3] - M2[m0-m3]
            "vsub.f32 q13, q1, q9      \n\t" // DST->M[m4-m7] = M1[m4-m7] - M2[m4-m7]
            "vsub.f32 q14, q2, q10     \n\t" // DST->M[m8-m11] = M1[m8-m11] - M2[m8-m11]
            "vsub.f32 q15, q3, q11     \n\t" // DST->M[m12-m15] = M1[m12-m15] - M2[m12-m15]

            "vst1.32 {q12, q13}, [%0]! \n\t" // DST->M[m0-m7]
            "vst1.32 {q14, q15}, [%0]  \n\t" // DST->M[m8-m15]
            :
            : "r"(dst.m), "r"(m1.m), "r"(m2.m)
            : "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "memory"
        );
#endif

#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif

#elif OUZEL_SUPPORTS_SSE
        dst.col[0] = _mm_sub_ps(m1.col[0], m2.col[0]);
        dst.col[1] = _mm_sub_ps(m1.col[1], m2.col[1]);
        dst.col[2] = _mm_sub_ps(m1.col[2], m2.col[2]);
        dst.col[3] = _mm_sub_ps(m1.col[3], m2.col[3]);
#endif

#if (!OUZEL_SUPPORTS_NEON && !OUZEL_SUPPORTS_SSE) || OUZEL_SUPPORTS_NEON_CHECK
#if OUZEL_SUPPORTS_NEON_CHECK
        else
        {
#endif
        dst.m[0]  = m1.m[0]  - m2.m[0];
        dst.m[1]  = m1.m[1]  - m2.m[1];
        dst.m[2]  = m1.m[2]  - m2.m[2];
        dst.m[3]  = m1.m[3]  - m2.m[3];
        dst.m[4]  = m1.m[4]  - m2.m[4];
        dst.m[5]  = m1.m[5]  - m2.m[5];
        dst.m[6]  = m1.m[6]  - m2.m[6];
        dst.m[7]  = m1.m[7]  - m2.m[7];
        dst.m[8]  = m1.m[8]  - m2.m[8];
        dst.m[9]  = m1.m[9]  - m2.m[9];
        dst.m[10] = m1.m[10] - m2.m[10];
        dst.m[11] = m1.m[11] - m2.m[11];
        dst.m[12] = m1.m[12] - m2.m[12];
        dst.m[13] = m1.m[13] - m2.m[13];
        dst.m[14] = m1.m[14] - m2.m[14];
        dst.m[15] = m1.m[15] - m2.m[15];
#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif
#endif
    }

    void Matrix4::transformVector(const Vector4& vector, Vector4& dst) const
    {
#if OUZEL_SUPPORTS_NEON
#if OUZEL_SUPPORTS_NEON_CHECK
        if (anrdoidNEONChecker.isNEONAvailable())
        {
#endif

#if OUZEL_64BITS // NEON64
        asm volatile
        (
            "ld1 {v0.4s}, [%1]                         \n\t" // V[x, y, z, w]
            "ld1 {v9.4s, v10.4s, v11.4s, v12.4s}, [%2] \n\t" // M[m0-m7] M[m8-m15]

            "fmul v13.4s, v9.4s, v0.s[0]               \n\t" // DST->V = M[m0-m3] * V[x]
            "fmla v13.4s, v10.4s, v0.s[1]              \n\t" // DST->V = M[m4-m7] * V[y]
            "fmla v13.4s, v11.4s, v0.s[2]              \n\t" // DST->V = M[m8-m11] * V[z]
            "fmla v13.4s, v12.4s, v0.s[3]              \n\t" // DST->V = M[m12-m15] * V[w]

            "st1 {v13.4s}, [%0]                        \n\t" // DST->V
            :
            : "r"(&dst.x), "r"(&vector.x), "r"(m)
            : "v0", "v9", "v10","v11", "v12", "v13", "memory"
        );
#else // NEON
        asm volatile
        (
            "vld1.32 {d0, d1}, [%1]      \n\t" // V[x, y, z, w]
            "vld1.32 {d18 - d21}, [%2]!  \n\t" // M[m0-m7]
            "vld1.32 {d22 - d25}, [%2]   \n\t" // M[m8-m15]

            "vmul.f32 q13, q9, d0[0]     \n\t" // DST->V = M[m0-m3] * V[x]
            "vmla.f32 q13, q10, d0[1]    \n\t" // DST->V = M[m4-m7] * V[y]
            "vmla.f32 q13, q11, d1[0]    \n\t" // DST->V = M[m8-m11] * V[z]
            "vmla.f32 q13, q12, d1[1]    \n\t" // DST->V = M[m12-m15] * V[w]

            "vst1.32    {d26, d27}, [%0] \n\t" // DST->V
            :
            : "r"(&dst.x), "r"(&vector.x), "r"(m)
            : "q0", "q9", "q10","q11", "q12", "q13", "memory"
         );
#endif

#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif

#elif OUZEL_SUPPORTS_SSE
        __m128 col1 = _mm_shuffle_ps(vector.s, vector.s, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 col2 = _mm_shuffle_ps(vector.s, vector.s, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 col3 = _mm_shuffle_ps(vector.s, vector.s, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 col4 = _mm_shuffle_ps(vector.s, vector.s, _MM_SHUFFLE(3, 3, 3, 3));

        dst.s = _mm_add_ps(_mm_add_ps(_mm_mul_ps(col[0], col1), _mm_mul_ps(col[1], col2)),
                           _mm_add_ps(_mm_mul_ps(col[2], col3), _mm_mul_ps(col[3], col4)));
#endif

#if (!OUZEL_SUPPORTS_NEON && !OUZEL_SUPPORTS_SSE) || OUZEL_SUPPORTS_NEON_CHECK
#if OUZEL_SUPPORTS_NEON_CHECK
        else
        {
#endif

        assert(&vector != &dst);
        dst.x = vector.x * m[0] + vector.y * m[4] + vector.z * m[8] + vector.w * m[12];
        dst.y = vector.x * m[1] + vector.y * m[5] + vector.z * m[9] + vector.w * m[13];
        dst.z = vector.x * m[2] + vector.y * m[6] + vector.z * m[10] + vector.w * m[14];
        dst.w = vector.x * m[3] + vector.y * m[7] + vector.z * m[11] + vector.w * m[15];
#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif
#endif
    }

    void Matrix4::translate(float x, float y, float z)
    {
        translate(x, y, z, *this);
    }

    void Matrix4::translate(float x, float y, float z, Matrix4& dst) const
    {
        Matrix4 t;
        createTranslation(x, y, z, t);
        multiply(*this, t, dst);
    }

    void Matrix4::translate(const Vector3& t)
    {
        translate(t.x, t.y, t.z, *this);
    }

    void Matrix4::translate(const Vector3& t, Matrix4& dst) const
    {
        translate(t.x, t.y, t.z, dst);
    }

    void Matrix4::transpose()
    {
        transpose(*this);
    }

    void Matrix4::transpose(Matrix4& dst) const
    {
#if OUZEL_SUPPORTS_NEON
#if OUZEL_SUPPORTS_NEON_CHECK
        if (anrdoidNEONChecker.isNEONAvailable())
        {
#endif

#if OUZEL_64BITS // NEON64
        asm volatile
        (
            "ld4 {v0.4s, v1.4s, v2.4s, v3.4s}, [%1] \n\t" // DST->M[m0, m4, m8, m12] = M[m0-m3]
            "st1 {v0.4s, v1.4s, v2.4s, v3.4s}, [%0] \n\t" // DST->M[m1, m5, m9, m12] = M[m4-m7]
            :
            : "r"(dst.m), "r"(m)
            : "v0", "v1", "v2", "v3", "memory"
        );
#else // NEON
        asm volatile
        (
            "vld4.32 {d0[0], d2[0], d4[0], d6[0]}, [%1]! \n\t" // DST->M[m0, m4, m8, m12] = M[m0-m3]
            "vld4.32 {d0[1], d2[1], d4[1], d6[1]}, [%1]! \n\t" // DST->M[m1, m5, m9, m12] = M[m4-m7]
            "vld4.32 {d1[0], d3[0], d5[0], d7[0]}, [%1]! \n\t" // DST->M[m2, m6, m10, m12] = M[m8-m11]
            "vld4.32 {d1[1], d3[1], d5[1], d7[1]}, [%1]  \n\t" // DST->M[m3, m7, m11, m12] = M[m12-m15]

            "vst1.32 {q0-q1}, [%0]!                      \n\t" // DST->M[m0-m7]
            "vst1.32 {q2-q3}, [%0]                       \n\t" // DST->M[m8-m15]
            :
            : "r"(dst.m), "r"(m)
            : "q0", "q1", "q2", "q3", "memory"
        );
#endif

#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif

#elif OUZEL_SUPPORTS_SSE
        __m128 tmp0 = _mm_shuffle_ps(col[0], col[1], 0x44);
        __m128 tmp2 = _mm_shuffle_ps(col[0], col[1], 0xEE);
        __m128 tmp1 = _mm_shuffle_ps(col[2], col[3], 0x44);
        __m128 tmp3 = _mm_shuffle_ps(col[2], col[3], 0xEE);

        dst.col[0] = _mm_shuffle_ps(tmp0, tmp1, 0x88);
        dst.col[1] = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
        dst.col[2] = _mm_shuffle_ps(tmp2, tmp3, 0x88);
        dst.col[3] = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
#endif

#if (!OUZEL_SUPPORTS_NEON && !OUZEL_SUPPORTS_SSE) || OUZEL_SUPPORTS_NEON_CHECK
#if OUZEL_SUPPORTS_NEON_CHECK
        else
        {
#endif
        float t[16] = {
            m[0], m[4], m[8], m[12],
            m[1], m[5], m[9], m[13],
            m[2], m[6], m[10], m[14],
            m[3], m[7], m[11], m[15]
        };
        std::copy(std::begin(t), std::end(t), dst.m);
#if OUZEL_SUPPORTS_NEON_CHECK
        }
#endif
#endif
    }

    Vector3 Matrix4::getTranslation() const
    {
        return Vector3(m[12], m[13], m[14]);
    }

    Vector3 Matrix4::getScale() const
    {
        Vector3 scale;
        scale.x = Vector3(m[0], m[1], m[2]).length();
        scale.y = Vector3(m[4], m[5], m[6]).length();
        scale.z = Vector3(m[8], m[9], m[10]).length();

        return scale;
    }

    Quaternion Matrix4::getRotation() const
    {
        Vector3 scale = getScale();

        float m11 = m[0] / scale.x;
        float m21 = m[1] / scale.x;
        float m31 = m[2] / scale.x;

        float m12 = m[4] / scale.y;
        float m22 = m[5] / scale.y;
        float m32 = m[6] / scale.y;

        float m13 = m[8] / scale.z;
        float m23 = m[9] / scale.z;
        float m33 = m[10] / scale.z;

        Quaternion result;
        result.x = sqrtf(std::max(0.0F, 1 + m11 - m22 - m33)) / 2.0F;
        result.y = sqrtf(std::max(0.0F, 1 - m11 + m22 - m33)) / 2.0F;
        result.z = sqrtf(std::max(0.0F, 1 - m11 - m22 + m33)) / 2.0F;
        result.w = sqrtf(std::max(0.0F, 1 + m11 + m22 + m33)) / 2.0F;

        result.x *= sgn(result.x * (m32 - m23));
        result.y *= sgn(result.y * (m13 - m31));
        result.z *= sgn(result.z * (m21 - m12));

        result.normalize();

        return result;
    }
}
