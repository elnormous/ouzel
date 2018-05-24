// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <algorithm>
#include "core/Setup.h"
#if OUZEL_SUPPORTS_SSE
#include <xmmintrin.h>
#endif
#include "math/ConvexVolume.hpp"
#include "math/Plane.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector3.hpp"
#include "math/Vector4.hpp"

namespace ouzel
{
    class Matrix4
    {
    public:
#if OUZEL_SUPPORTS_SSE
        union
        {
            __m128 col[4];
            float m[16];
        };
#else
        float m[16];
#endif

        Matrix4()
        {
            std::fill(std::begin(m), std::end(m), 0.0F);
        }

        Matrix4(float m11, float m12, float m13, float m14,
                float m21, float m22, float m23, float m24,
                float m31, float m32, float m33, float m34,
                float m41, float m42, float m43, float m44);

        Matrix4(const float* array);
        Matrix4(const Matrix4& copy);

        float& operator[](size_t index) { return m[index]; }
        float operator[](size_t index) const { return m[index]; }

        static Matrix4 identity()
        {
            return Matrix4(1.0F, 0.0F, 0.0F, 0.0F,
                           0.0F, 1.0F, 0.0F, 0.0F,
                           0.0F, 0.0F, 1.0F, 0.0F,
                           0.0F, 0.0F, 0.0F, 1.0F);
        }

        static void createLookAt(const Vector3& eyePosition, const Vector3& targetPosition, const Vector3& up, Matrix4& dst);
        static void createLookAt(float eyePositionX, float eyePositionY, float eyePositionZ,
                                 float targetCenterX, float targetCenterY, float targetCenterZ,
                                 float upX, float upY, float upZ, Matrix4& dst);
        static void createPerspective(float fieldOfView, float aspectRatio, float zNearPlane, float zFarPlane, Matrix4& dst);

        static void createOrthographicFromSize(float width, float height, float zNearPlane, float zFarPlane, Matrix4& dst);
        static void createOrthographicOffCenter(float left, float right, float bottom, float top,
                                                float zNearPlane, float zFarPlane, Matrix4& dst);
        static void createScale(const Vector3& scale, Matrix4& dst);
        static void createScale(float xScale, float yScale, float zScale, Matrix4& dst);
        static void createRotation(const Vector3& axis, float angle, Matrix4& dst);
        static void createRotationX(float angle, Matrix4& dst);
        static void createRotationY(float angle, Matrix4& dst);
        static void createRotationZ(float angle, Matrix4& dst);
        static void createTranslation(const Vector3& translation, Matrix4& dst);
        static void createTranslation(float xTranslation, float yTranslation, float zTranslation, Matrix4& dst);

        bool getFrustumLeftPlane(Plane& plane) const
        {
            return Plane::makeFrustumPlane(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12], plane);
        }

        bool getFrustumRightPlane(Plane& plane) const
        {
            return Plane::makeFrustumPlane(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12], plane);
        }

        bool getFrustumBottomPlane(Plane& plane) const
        {
            return Plane::makeFrustumPlane(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13], plane);
        }

        bool getFrustumTopPlane(Plane& plane) const
        {
            return Plane::makeFrustumPlane(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13], plane);
        }

        bool getFrustumNearPlane(Plane& plane) const
        {
            return Plane::makeFrustumPlane(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14], plane);
        }

        bool getFrustumFarPlane(Plane& plane) const
        {
            return Plane::makeFrustumPlane(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14], plane);
        }

        bool getFrustum(ConvexVolume& frustum) const
        {
            frustum.planes.clear();
            Plane plane;

            if (!getFrustumLeftPlane(plane)) return false;
            frustum.planes.push_back(plane);

            if (!getFrustumRightPlane(plane)) return false;
            frustum.planes.push_back(plane);

            if (!getFrustumBottomPlane(plane)) return false;
            frustum.planes.push_back(plane);

            if (!getFrustumTopPlane(plane)) return false;
            frustum.planes.push_back(plane);

            if (!getFrustumNearPlane(plane)) return false;
            frustum.planes.push_back(plane);

            if (!getFrustumFarPlane(plane)) return false;
            frustum.planes.push_back(plane);

            return true;
        }

        void add(float scalar);
        void add(float scalar, Matrix4& dst);
        void add(const Matrix4& matrix);
        static void add(const Matrix4& m1, const Matrix4& m2, Matrix4& dst);

        float determinant() const;

        void getUpVector(Vector3& dst) const;
        void getDownVector(Vector3& dst) const;
        void getLeftVector(Vector3& dst) const;
        void getRightVector(Vector3& dst) const;
        void getForwardVector(Vector3& dst) const;
        void getBackVector(Vector3& dst) const;

        bool invert();
        bool invert(Matrix4& dst) const;

        inline bool isIdentity() const
        {
            return m[0]  == 1.0F && m[1]  == 0.0F && m[2]  == 0.0F && m[3]  == 0.0F &&
                   m[4]  == 0.0F && m[5]  == 1.0F && m[6]  == 0.0F && m[7]  == 0.0F &&
                   m[8]  == 0.0F && m[9]  == 0.0F && m[10] == 1.0F && m[11] == 0.0F &&
                   m[12] == 0.0F && m[13] == 0.0F && m[14] == 0.0F && m[15] == 1.0F;
        }

        void multiply(float scalar);
        void multiply(float scalar, Matrix4& dst) const;
        static void multiply(const Matrix4& m, float scalar, Matrix4& dst);
        void multiply(const Matrix4& matrix);
        static void multiply(const Matrix4& m1, const Matrix4& m2, Matrix4& dst);

        void negate();
        void negate(Matrix4& dst) const;

        void rotate(const Vector3& axis, float angle);
        void rotate(const Vector3& axis, float angle, Matrix4& dst) const;
        void rotateX(float angle);
        void rotateX(float angle, Matrix4& dst) const;
        void rotateY(float angle);
        void rotateY(float angle, Matrix4& dst) const;
        void rotateZ(float angle);
        void rotateZ(float angle, Matrix4& dst) const;

        void scale(float value);
        void scale(float value, Matrix4& dst) const;
        void scale(float xScale, float yScale, float zScale);
        void scale(float xScale, float yScale, float zScale, Matrix4& dst) const;
        void scale(const Vector3& s);
        void scale(const Vector3& s, Matrix4& dst) const;

        void set(float m11, float m12, float m13, float m14,
                 float m21, float m22, float m23, float m24,
                 float m31, float m32, float m33, float m34,
                 float m41, float m42, float m43, float m44);
        void set(const float* array);

        inline void setIdentity()
        {
            m[0]  = 1.0F; m[1] = 0.0F;  m[2]  = 0.0F; m[3]  = 0.0F;
            m[4]  = 0.0F; m[5] = 1.0F;  m[6]  = 0.0F; m[7]  = 0.0F;
            m[8]  = 0.0F; m[9] = 0.0F;  m[10] = 1.0F; m[11] = 0.0F;
            m[12] = 0.0F; m[13] = 0.0F; m[14] = 0.0F; m[15] = 1.0F;
        }

        inline void setZero()
        {
            std::fill(m, m + sizeof(m) / sizeof(float), 0.0F);
        }

        void subtract(const Matrix4& matrix);
        static void subtract(const Matrix4& m1, const Matrix4& m2, Matrix4& dst);

        void transformPoint(Vector3& point) const
        {
            transformVector(point.x, point.y, point.z, 1.0F, point);
        }

        void transformPoint(const Vector3& point, Vector3& dst) const
        {
            transformVector(point.x, point.y, point.z, 1.0F, dst);
        }

        void transformVector(Vector3& vector) const
        {
            Vector4 t;
            transformVector(Vector4(vector.x, vector.y, vector.z, 0.0F), t);
            vector = Vector3(t.x, t.y, t.z);
        }

        void transformVector(const Vector3& vector, Vector3& dst) const
        {
            transformVector(vector.x, vector.y, vector.z, 0.0F, dst);
        }

        void transformVector(float x, float y, float z, float w, Vector3& dst) const
        {
            Vector4 t;
            transformVector(Vector4(x, y, z, w), t);
            dst = Vector3(t.x, t.y, t.z);
        }

        void transformVector(Vector4& vector) const
        {
            transformVector(vector, vector);
        }

        void transformVector(const Vector4& vector, Vector4& dst) const;

        void translate(float x, float y, float z);
        void translate(float x, float y, float z, Matrix4& dst) const;
        void translate(const Vector3& t);
        void translate(const Vector3& t, Matrix4& dst) const;

        void transpose();
        void transpose(Matrix4& dst) const;

        Vector3 getTranslation() const;
        Vector3 getScale() const;
        Quaternion getRotation() const;

        inline Matrix4 operator+(const Matrix4& matrix) const
        {
            Matrix4 result(*this);
            add(result, matrix, result);
            return result;
        }

        inline Matrix4& operator+=(const Matrix4& matrix)
        {
            add(matrix);
            return *this;
        }

        inline Matrix4 operator-(const Matrix4& matrix) const
        {
            Matrix4 result(*this);
            subtract(result, matrix, result);
            return result;
        }

        inline Matrix4& operator-=(const Matrix4& matrix)
        {
            subtract(matrix);
            return *this;
        }

        inline Matrix4 operator-() const
        {
            Matrix4 result(*this);
            negate(result);
            return result;
        }

        inline Matrix4 operator*(const Matrix4& matrix) const
        {
            Matrix4 result(*this);
            multiply(result, matrix, result);
            return result;
        }

        inline Matrix4& operator*=(const Matrix4& matrix)
        {
            multiply(matrix);
            return *this;
        }

        inline bool operator==(const Matrix4& matrix)
        {
            return m[0] == matrix.m[0] &&
                   m[1] == matrix.m[1] &&
                   m[2] == matrix.m[2] &&
                   m[3] == matrix.m[3] &&
                   m[4] == matrix.m[4] &&
                   m[5] == matrix.m[5] &&
                   m[6] == matrix.m[6] &&
                   m[7] == matrix.m[7] &&
                   m[8] == matrix.m[8] &&
                   m[9] == matrix.m[9] &&
                   m[10] == matrix.m[10] &&
                   m[11] == matrix.m[11] &&
                   m[12] == matrix.m[12] &&
                   m[13] == matrix.m[13] &&
                   m[14] == matrix.m[14] &&
                   m[15] == matrix.m[15];
        }

        inline bool operator!=(const Matrix4& matrix)
        {
            return m[0] != matrix.m[0] ||
                   m[1] != matrix.m[1] ||
                   m[2] != matrix.m[2] ||
                   m[3] != matrix.m[3] ||
                   m[4] != matrix.m[4] ||
                   m[5] != matrix.m[5] ||
                   m[6] != matrix.m[6] ||
                   m[7] != matrix.m[7] ||
                   m[8] != matrix.m[8] ||
                   m[9] != matrix.m[9] ||
                   m[10] != matrix.m[10] ||
                   m[11] != matrix.m[11] ||
                   m[12] != matrix.m[12] ||
                   m[13] != matrix.m[13] ||
                   m[14] != matrix.m[14] ||
                   m[15] != matrix.m[15];
        }

        void setRotation(const Quaternion& rotation)
        {
            Matrix4 result;

            float wx = rotation.w * rotation.x;
            float wy = rotation.w * rotation.y;
            float wz = rotation.w * rotation.z;

            float xx = rotation.x * rotation.x;
            float xy = rotation.x * rotation.y;
            float xz = rotation.x * rotation.z;

            float yy = rotation.y * rotation.y;
            float yz = rotation.y * rotation.z;

            float zz = rotation.z * rotation.z;

            m[0] = 1.0F - 2.0F * (yy + zz);
            m[4] = 2.0F * (xy - wz);
            m[8] = 2.0F * (xz + wy);
            m[12] = 0.0F;

            m[1] = 2.0F * (xy + wz);
            m[5] = 1.0F - 2.0F * (xx + zz);
            m[9] = 2.0F * (yz - wx);
            m[13] = 0.0F;

            m[2] = 2.0F * (xz - wy);
            m[6] = 2.0F * (yz + wx);
            m[10] = 1.0F - 2.0F * (xx + yy);
            m[14] = 0.0F;

            m[3] = 0.0F;
            m[7] = 0.0F;
            m[11] = 0.0F;
            m[15] = 1.0F;
        }
    };

    inline Vector3& operator*=(Vector3& v, const Matrix4& m)
    {
        m.transformVector(v);
        return v;
    }

    inline Vector3 operator*(const Matrix4& m, const Vector3& v)
    {
        Vector3 x;
        m.transformVector(v, x);
        return x;
    }

    inline Vector4& operator*=(Vector4& v, const Matrix4& m)
    {
        m.transformVector(v);
        return v;
    }

    inline Vector4 operator*(const Matrix4& m, const Vector4& v)
    {
        Vector4 x;
        m.transformVector(v, x);
        return x;
    }
}
