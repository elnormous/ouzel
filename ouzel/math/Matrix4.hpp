// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_MATRIX4_HPP
#define OUZEL_MATH_MATRIX4_HPP

#include <algorithm>
#if defined(__SSE__)
#  include <xmmintrin.h>
#endif
#include "math/ConvexVolume.hpp"
#include "math/Plane.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector3.hpp"
#include "math/Vector4.hpp"

namespace ouzel
{
    class Matrix4 final
    {
    public:
#if defined(__SSE__)
        alignas(16)
#endif
        float m[16];

        Matrix4()
        {
            std::fill(std::begin(m), std::end(m), 0.0F);
        }

        Matrix4(float m11, float m12, float m13, float m14,
                float m21, float m22, float m23, float m24,
                float m31, float m32, float m33, float m34,
                float m41, float m42, float m43, float m44)
        {
            set(m11, m12, m13, m14,
                m21, m22, m23, m24,
                m31, m32, m33, m34,
                m41, m42, m43, m44);
        }

        explicit Matrix4(const float* array)
        {
            set(array);
        }

        Matrix4(const Matrix4& copy)
        {
            std::copy(std::begin(copy.m), std::end(copy.m), m);
        }

        float& operator[](size_t index) { return m[index]; }
        float operator[](size_t index) const { return m[index]; }

        static inline Matrix4 identity()
        {
            return Matrix4(1.0F, 0.0F, 0.0F, 0.0F,
                           0.0F, 1.0F, 0.0F, 0.0F,
                           0.0F, 0.0F, 1.0F, 0.0F,
                           0.0F, 0.0F, 0.0F, 1.0F);
        }

        static void createLookAt(const Vector3& eyePosition, const Vector3& targetPosition, const Vector3& up, Matrix4& dst);
        static void createLookAt(float eyePositionX, float eyePositionY, float eyePositionZ,
                                 float targetPositionX, float targetPositionY, float targetPositionZ,
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

        Plane getFrustumLeftPlane() const
        {
            return Plane::makeFrustumPlane(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
        }

        inline Plane getFrustumRightPlane() const
        {
            return Plane::makeFrustumPlane(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
        }

        inline Plane getFrustumBottomPlane() const
        {
            return Plane::makeFrustumPlane(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13]);
        }

        inline Plane getFrustumTopPlane() const
        {
            return Plane::makeFrustumPlane(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13]);
        }

        inline Plane getFrustumNearPlane() const
        {
            return Plane::makeFrustumPlane(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14]);
        }

        inline Plane getFrustumFarPlane() const
        {
            return Plane::makeFrustumPlane(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14]);
        }

        ConvexVolume getFrustum() const
        {
            ConvexVolume frustum;
            frustum.planes.push_back(getFrustumLeftPlane());
            frustum.planes.push_back(getFrustumRightPlane());
            frustum.planes.push_back(getFrustumBottomPlane());
            frustum.planes.push_back(getFrustumTopPlane());
            frustum.planes.push_back(getFrustumNearPlane());
            frustum.planes.push_back(getFrustumFarPlane());
            return frustum;
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

        void invert();
        void invert(Matrix4& dst) const;

        inline bool isIdentity() const
        {
            return m[0] == 1.0F && m[1] == 0.0F && m[2] == 0.0F && m[3] == 0.0F &&
                   m[4] == 0.0F && m[5] == 1.0F && m[6] == 0.0F && m[7] == 0.0F &&
                   m[8] == 0.0F && m[9] == 0.0F && m[10] == 1.0F && m[11] == 0.0F &&
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
            m[0] = 1.0F; m[1] = 0.0F; m[2] = 0.0F; m[3] = 0.0F;
            m[4] = 0.0F; m[5] = 1.0F; m[6] = 0.0F; m[7] = 0.0F;
            m[8] = 0.0F; m[9] = 0.0F; m[10] = 1.0F; m[11] = 0.0F;
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
            transformVector(point.v[0], point.v[1], point.v[2], 1.0F, point);
        }

        void transformPoint(const Vector3& point, Vector3& dst) const
        {
            transformVector(point.v[0], point.v[1], point.v[2], 1.0F, dst);
        }

        void transformVector(Vector3& vector) const
        {
            Vector4 t;
            transformVector(Vector4(vector.v[0], vector.v[1], vector.v[2], 0.0F), t);
            vector = Vector3(t.v[0], t.v[1], t.v[2]);
        }

        void transformVector(const Vector3& vector, Vector3& dst) const
        {
            transformVector(vector.v[0], vector.v[1], vector.v[2], 0.0F, dst);
        }

        void transformVector(float x, float y, float z, float w, Vector3& dst) const
        {
            Vector4 t;
            transformVector(Vector4(x, y, z, w), t);
            dst = Vector3(t.v[0], t.v[1], t.v[2]);
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

        Vector3 getTranslation() const
        {
            return Vector3(m[12], m[13], m[14]);
        }

        Vector3 getScale() const
        {
            Vector3 scale;
            scale.v[0] = Vector3(m[0], m[1], m[2]).length();
            scale.v[1] = Vector3(m[4], m[5], m[6]).length();
            scale.v[2] = Vector3(m[8], m[9], m[10]).length();

            return scale;
        }

        Quaternion getRotation() const
        {
            Vector3 scale = getScale();

            float m11 = m[0] / scale.v[0];
            float m21 = m[1] / scale.v[0];
            float m31 = m[2] / scale.v[0];

            float m12 = m[4] / scale.v[1];
            float m22 = m[5] / scale.v[1];
            float m32 = m[6] / scale.v[1];

            float m13 = m[8] / scale.v[2];
            float m23 = m[9] / scale.v[2];
            float m33 = m[10] / scale.v[2];

            Quaternion result;
            result.v[0] = sqrtf(std::max(0.0F, 1 + m11 - m22 - m33)) / 2.0F;
            result.v[1] = sqrtf(std::max(0.0F, 1 - m11 + m22 - m33)) / 2.0F;
            result.v[2] = sqrtf(std::max(0.0F, 1 - m11 - m22 + m33)) / 2.0F;
            result.v[3] = sqrtf(std::max(0.0F, 1 + m11 + m22 + m33)) / 2.0F;

            result.v[0] *= sgn(result.v[0] * (m32 - m23));
            result.v[1] *= sgn(result.v[1] * (m13 - m31));
            result.v[2] *= sgn(result.v[2] * (m21 - m12));

            result.normalize();

            return result;
        }

        void setRotation(const Quaternion& rotation)
        {
            float wx = rotation.v[3] * rotation.v[0];
            float wy = rotation.v[3] * rotation.v[1];
            float wz = rotation.v[3] * rotation.v[2];

            float xx = rotation.v[0] * rotation.v[0];
            float xy = rotation.v[0] * rotation.v[1];
            float xz = rotation.v[0] * rotation.v[2];

            float yy = rotation.v[1] * rotation.v[1];
            float yz = rotation.v[1] * rotation.v[2];

            float zz = rotation.v[2] * rotation.v[2];

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

        inline const Matrix4 operator+(const Matrix4& matrix) const
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

        inline const Matrix4 operator-(const Matrix4& matrix) const
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

        inline const Matrix4 operator-() const
        {
            Matrix4 result(*this);
            negate(result);
            return result;
        }

        inline const Matrix4 operator*(const Matrix4& matrix) const
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
    };

    inline Vector3& operator*=(Vector3& v, const Matrix4& m)
    {
        m.transformVector(v);
        return v;
    }

    inline const Vector3 operator*(const Matrix4& m, const Vector3& v)
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

    inline const Vector4 operator*(const Matrix4& m, const Vector4& v)
    {
        Vector4 x;
        m.transformVector(v, x);
        return x;
    }
}

#endif // OUZEL_MATH_MATRIX4_HPP
