// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_MATRIX4_HPP
#define OUZEL_MATH_MATRIX4_HPP

#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>
#include "math/ConvexVolume.hpp"
#include "math/Plane.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector3.hpp"
#include "math/Vector4.hpp"

namespace ouzel
{
    template<class T> class Matrix4 final
    {
    public:
#if defined(__SSE__)
        alignas(16)
#endif
        T m[16]{0};

        Matrix4()
        {
        }

        Matrix4(T m11, T m12, T m13, T m14,
                T m21, T m22, T m23, T m24,
                T m31, T m32, T m33, T m34,
                T m41, T m42, T m43, T m44)
        {
            m[0] = m11;
            m[1] = m21;
            m[2] = m31;
            m[3] = m41;
            m[4] = m12;
            m[5] = m22;
            m[6] = m32;
            m[7] = m42;
            m[8] = m13;
            m[9] = m23;
            m[10] = m33;
            m[11] = m43;
            m[12] = m14;
            m[13] = m24;
            m[14] = m34;
            m[15] = m44;
        }

        explicit Matrix4(const T* array)
        {
            m[0] = array[0];
            m[1] = array[1];
            m[2] = array[2];
            m[3] = array[3];
            m[4] = array[4];
            m[5] = array[5];
            m[6] = array[6];
            m[7] = array[7];
            m[8] = array[8];
            m[9] = array[9];
            m[10] = array[10];
            m[11] = array[11];
            m[12] = array[12];
            m[13] = array[13];
            m[14] = array[14];
            m[15] = array[15];
        }

        T& operator[](size_t index) { return m[index]; }
        T operator[](size_t index) const { return m[index]; }

        static inline constexpr Matrix4 identity()
        {
            return Matrix4(1, 0, 0, 0,
                           0, 1, 0, 0,
                           0, 0, 1, 0,
                           0, 0, 0, 1);
        }

        static void createLookAt(const Vector3<T>& eyePosition,
                                 const Vector3<T>& targetPosition,
                                 const Vector3<T>& up, Matrix4& dst)
        {
            createLookAt(eyePosition.v[0], eyePosition.v[1], eyePosition.v[2],
                         targetPosition.v[0], targetPosition.v[1], targetPosition.v[2],
                         up.v[0], up.v[1], up.v[2], dst);
        }

        static void createLookAt(T eyePositionX, T eyePositionY, T eyePositionZ,
                                 T targetPositionX, T targetPositionY, T targetPositionZ,
                                 T upX, T upY, T upZ, Matrix4& dst)
        {
            Vector3<T> eye(eyePositionX, eyePositionY, eyePositionZ);
            Vector3<T> target(targetPositionX, targetPositionY, targetPositionZ);
            Vector3<T> up(upX, upY, upZ);
            up.normalize();

            Vector3<T> zaxis = target - eye;
            zaxis.normalize();

            Vector3<T> xaxis = up.cross(zaxis);
            xaxis.normalize();

            Vector3<T> yaxis = zaxis.cross(xaxis);
            yaxis.normalize();

            dst.m[0] = xaxis.v[0];
            dst.m[1] = yaxis.v[0];
            dst.m[2] = zaxis.v[0];
            dst.m[3] = 0.0F;

            dst.m[4] = xaxis.v[1];
            dst.m[5] = yaxis.v[1];
            dst.m[6] = zaxis.v[1];
            dst.m[7] = 0.0F;

            dst.m[8] = xaxis.v[2];
            dst.m[9] = yaxis.v[2];
            dst.m[10] = zaxis.v[2];
            dst.m[11] = 0.0F;

            dst.m[12] = xaxis.dot(-eye);
            dst.m[13] = yaxis.dot(-eye);
            dst.m[14] = zaxis.dot(-eye);
            dst.m[15] = 1.0F;
        }

        static void createPerspective(T fieldOfView, T aspectRatio, T zNearPlane, T zFarPlane, Matrix4& dst)
        {
            assert(zFarPlane != zNearPlane);

            float theta = fieldOfView * 0.5F;
            if (isNearlyEqual(fmodf(theta, PI / 2.0F), 0.0F)) // invalid field of view value
                return;

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

        static void createOrthographicFromSize(T width, T height, T zNearPlane, T zFarPlane, Matrix4& dst)
        {
            float halfWidth = width / 2.0F;
            float halfHeight = height / 2.0F;
            createOrthographicOffCenter(-halfWidth, halfWidth,
                                        -halfHeight, halfHeight,
                                        zNearPlane, zFarPlane, dst);
        }

        static void createOrthographicOffCenter(T left, T right, T bottom, T top,
                                                T zNearPlane, T zFarPlane, Matrix4& dst)
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

        static void createScale(const Vector3<T>& scale, Matrix4& dst)
        {
            dst.setIdentity();

            dst.m[0] = scale.v[0];
            dst.m[5] = scale.v[1];
            dst.m[10] = scale.v[2];
        }

        static void createScale(T xScale, T yScale, T zScale, Matrix4& dst)
        {
            dst.setIdentity();

            dst.m[0] = xScale;
            dst.m[5] = yScale;
            dst.m[10] = zScale;
        }

        static void createRotation(const Vector3<T>& axis, float angle, Matrix4& dst)
        {
            float x = axis.v[0];
            float y = axis.v[1];
            float z = axis.v[2];

            // Make sure the input axis is normalized
            float n = x * x + y * y + z * z;
            if (n != 1.0F)
            {
                // Not normalized
                n = sqrtf(n);
                // Prevent divide too close to zero
                if (n >= std::numeric_limits<float>::min())
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

        static void createRotationX(float angle, Matrix4& dst)
        {
            dst.setIdentity();

            float c = cosf(angle);
            float s = sinf(angle);

            dst.m[5] = c;
            dst.m[9] = -s;
            dst.m[6] = s;
            dst.m[10] = c;
        }

        static void createRotationY(float angle, Matrix4& dst)
        {
            dst.setIdentity();

            float c = cosf(angle);
            float s = sinf(angle);

            dst.m[0] = c;
            dst.m[8] = s;
            dst.m[2] = -s;
            dst.m[10] = c;
        }

        static void createRotationZ(float angle, Matrix4& dst)
        {
            dst.setIdentity();

            float c = cosf(angle);
            float s = sinf(angle);

            dst.m[0] = c;
            dst.m[4] = -s;
            dst.m[1] = s;
            dst.m[5] = c;
        }

        static void createTranslation(const Vector3<T>& translation, Matrix4& dst)
        {
            dst.setIdentity();

            dst.m[12] = translation.v[0];
            dst.m[13] = translation.v[1];
            dst.m[14] = translation.v[2];
        }

        static void createTranslation(T xTranslation, T yTranslation, T zTranslation, Matrix4& dst)
        {
            dst.setIdentity();

            dst.m[12] = xTranslation;
            dst.m[13] = yTranslation;
            dst.m[14] = zTranslation;
        }

        Plane<T> getFrustumLeftPlane() const
        {
            return Plane<T>::makeFrustumPlane(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
        }

        inline Plane<T> getFrustumRightPlane() const
        {
            return Plane<T>::makeFrustumPlane(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
        }

        inline Plane<T> getFrustumBottomPlane() const
        {
            return Plane<T>::makeFrustumPlane(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13]);
        }

        inline Plane<T> getFrustumTopPlane() const
        {
            return Plane<T>::makeFrustumPlane(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13]);
        }

        inline Plane<T> getFrustumNearPlane() const
        {
            return Plane<T>::makeFrustumPlane(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14]);
        }

        inline Plane<T> getFrustumFarPlane() const
        {
            return Plane<T>::makeFrustumPlane(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14]);
        }

        ConvexVolume<T> getFrustum() const
        {
            ConvexVolume<T> frustum;
            frustum.planes.push_back(getFrustumLeftPlane());
            frustum.planes.push_back(getFrustumRightPlane());
            frustum.planes.push_back(getFrustumBottomPlane());
            frustum.planes.push_back(getFrustumTopPlane());
            frustum.planes.push_back(getFrustumNearPlane());
            frustum.planes.push_back(getFrustumFarPlane());
            return frustum;
        }

        void add(T scalar)
        {
            add(scalar, *this);
        }

        void add(T scalar, Matrix4& dst);

        void add(const Matrix4& matrix)
        {
            add(*this, matrix, *this);
        }

        static void add(const Matrix4& m1, const Matrix4& m2, Matrix4& dst);

        float determinant() const
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

        inline void getUpVector(Vector3<T>& dst) const
        {
            dst.v[0] = m[4];
            dst.v[1] = m[5];
            dst.v[2] = m[6];
        }

        inline void getDownVector(Vector3<T>& dst) const
        {
            dst.v[0] = -m[4];
            dst.v[1] = -m[5];
            dst.v[2] = -m[6];
        }

        inline void getLeftVector(Vector3<T>& dst) const
        {
            dst.v[0] = -m[0];
            dst.v[1] = -m[1];
            dst.v[2] = -m[2];
        }

        inline void getRightVector(Vector3<T>& dst) const
        {
            dst.v[0] = m[0];
            dst.v[1] = m[1];
            dst.v[2] = m[2];
        }

        inline void getForwardVector(Vector3<T>& dst) const
        {
            dst.v[0] = -m[8];
            dst.v[1] = -m[9];
            dst.v[2] = -m[10];
        }

        inline void getBackVector(Vector3<T>& dst) const
        {
            dst.v[0] = m[8];
            dst.v[1] = m[9];
            dst.v[2] = m[10];
        }

        void invert()
        {
            invert(*this);
        }

        void invert(Matrix4& dst) const;

        inline bool isIdentity() const
        {
            return m[0] == 1.0F && m[1] == 0.0F && m[2] == 0.0F && m[3] == 0.0F &&
                   m[4] == 0.0F && m[5] == 1.0F && m[6] == 0.0F && m[7] == 0.0F &&
                   m[8] == 0.0F && m[9] == 0.0F && m[10] == 1.0F && m[11] == 0.0F &&
                   m[12] == 0.0F && m[13] == 0.0F && m[14] == 0.0F && m[15] == 1.0F;
        }

        void multiply(T scalar)
        {
            multiply(scalar, *this);
        }

        void multiply(T scalar, Matrix4& dst) const
        {
            multiply(*this, scalar, dst);
        }

        static void multiply(const Matrix4& m, T scalar, Matrix4& dst);

        void multiply(const Matrix4& matrix)
        {
            multiply(*this, matrix, *this);
        }

        static void multiply(const Matrix4& m1, const Matrix4& m2, Matrix4& dst);

        void negate()
        {
            negate(*this);
        }

        void negate(Matrix4& dst) const;

        void rotate(const Vector3<T>& axis, float angle)
        {
            rotate(axis, angle, *this);
        }

        void rotate(const Vector3<T>& axis, float angle, Matrix4& dst) const
        {
            Matrix4 r;
            createRotation(axis, angle, r);
            multiply(*this, r, dst);
        }

        void rotateX(float angle)
        {
            rotateX(angle, *this);
        }

        void rotateX(float angle, Matrix4& dst) const
        {
            Matrix4 r;
            createRotationX(angle, r);
            multiply(*this, r, dst);
        }

        void rotateY(float angle)
        {
            rotateY(angle, *this);
        }

        void rotateY(float angle, Matrix4& dst) const
        {
            Matrix4 r;
            createRotationY(angle, r);
            multiply(*this, r, dst);
        }

        void rotateZ(float angle)
        {
            rotateZ(angle, *this);
        }

        void rotateZ(float angle, Matrix4& dst) const
        {
            Matrix4 r;
            createRotationZ(angle, r);
            multiply(*this, r, dst);
        }

        void scale(T value)
        {
            scale(value, *this);
        }

        void scale(T value, Matrix4& dst) const
        {
            scale(value, value, value, dst);
        }

        void scale(T xScale, T yScale, T zScale)
        {
            scale(xScale, yScale, zScale, *this);
        }

        void scale(T xScale, T yScale, T zScale, Matrix4& dst) const
        {
            Matrix4 s;
            createScale(xScale, yScale, zScale, s);
            multiply(*this, s, dst);
        }

        void scale(const Vector3<T>& s)
        {
            scale(s.v[0], s.v[1], s.v[2], *this);
        }

        void scale(const Vector3<T>& s, Matrix4& dst) const
        {
            scale(s.v[0], s.v[1], s.v[2], dst);
        }

        inline void setIdentity()
        {
            m[0] = 1.0F; m[1] = 0.0F; m[2] = 0.0F; m[3] = 0.0F;
            m[4] = 0.0F; m[5] = 1.0F; m[6] = 0.0F; m[7] = 0.0F;
            m[8] = 0.0F; m[9] = 0.0F; m[10] = 1.0F; m[11] = 0.0F;
            m[12] = 0.0F; m[13] = 0.0F; m[14] = 0.0F; m[15] = 1.0F;
        }

        inline void setZero()
        {
            std::fill(m, m + sizeof(m) / sizeof(float), static_cast<T>(0));
        }

        void subtract(const Matrix4& matrix)
        {
            subtract(*this, matrix, *this);
        }

        static void subtract(const Matrix4& m1, const Matrix4& m2, Matrix4& dst);

        void transformPoint(Vector3<T>& point) const
        {
            transformVector(point.v[0], point.v[1], point.v[2], 1.0F, point);
        }

        void transformPoint(const Vector3<T>& point, Vector3<T>& dst) const
        {
            transformVector(point.v[0], point.v[1], point.v[2], 1.0F, dst);
        }

        void transformVector(Vector3<T>& vector) const
        {
            Vector4<T> t;
            transformVector(Vector4<T>(vector.v[0], vector.v[1], vector.v[2], 0.0F), t);
            vector = Vector3<T>(t.v[0], t.v[1], t.v[2]);
        }

        void transformVector(const Vector3<T>& vector, Vector3<T>& dst) const
        {
            transformVector(vector.v[0], vector.v[1], vector.v[2], 0.0F, dst);
        }

        void transformVector(T x, T y, T z, T w, Vector3<T>& dst) const
        {
            Vector4<T> t;
            transformVector(Vector4<T>(x, y, z, w), t);
            dst = Vector3<T>(t.v[0], t.v[1], t.v[2]);
        }

        void transformVector(Vector4<T>& vector) const
        {
            transformVector(vector, vector);
        }

        void transformVector(const Vector4<T>& vector, Vector4<T>& dst) const;

        void translate(float x, float y, float z)
        {
            translate(x, y, z, *this);
        }

        void translate(T x, T y, T z, Matrix4& dst) const
        {
            Matrix4 t;
            createTranslation(x, y, z, t);
            multiply(*this, t, dst);
        }

        void translate(const Vector3<T>& t)
        {
            translate(t.v[0], t.v[1], t.v[2], *this);
        }

        void translate(const Vector3<T>& t, Matrix4& dst) const
        {
            translate(t.v[0], t.v[1], t.v[2], dst);
        }

        void transpose()
        {
            transpose(*this);
        }

        void transpose(Matrix4& dst) const;

        Vector3<T> getTranslation() const
        {
            return Vector3<T>(m[12], m[13], m[14]);
        }

        Vector3<T> getScale() const
        {
            Vector3<T> scale;
            scale.v[0] = Vector3<T>(m[0], m[1], m[2]).length();
            scale.v[1] = Vector3<T>(m[4], m[5], m[6]).length();
            scale.v[2] = Vector3<T>(m[8], m[9], m[10]).length();

            return scale;
        }

        Quaternion<T> getRotation() const
        {
            Vector3<T> scale = getScale();

            float m11 = m[0] / scale.v[0];
            float m21 = m[1] / scale.v[0];
            float m31 = m[2] / scale.v[0];

            float m12 = m[4] / scale.v[1];
            float m22 = m[5] / scale.v[1];
            float m32 = m[6] / scale.v[1];

            float m13 = m[8] / scale.v[2];
            float m23 = m[9] / scale.v[2];
            float m33 = m[10] / scale.v[2];

            Quaternion<T> result;
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

        void setRotation(const Quaternion<T>& rotation)
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

    template<class T>
    inline Vector3<T>& operator*=(Vector3<T>& v, const Matrix4<T>& m)
    {
        m.transformVector(v);
        return v;
    }

    template<class T>
    inline const Vector3<T> operator*(const Matrix4<T>& m, const Vector3<T>& v)
    {
        Vector3<T> x;
        m.transformVector(v, x);
        return x;
    }

    template<class T>
    inline Vector4<T>& operator*=(Vector4<T>& v, const Matrix4<T>& m)
    {
        m.transformVector(v);
        return v;
    }

    template<class T>
    inline const Vector4<T> operator*(const Matrix4<T>& m, const Vector4<T>& v)
    {
        Vector4<T> x;
        m.transformVector(v, x);
        return x;
    }

    using Matrix4F = Matrix4<float>;
}

#endif // OUZEL_MATH_MATRIX4_HPP
