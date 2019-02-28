// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_MATRIX_HPP
#define OUZEL_MATH_MATRIX_HPP

#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>
#include "math/ConvexVolume.hpp"
#include "math/Plane.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector.hpp"

namespace ouzel
{
    template<size_t N, size_t M, class T> class Matrix final
    {
    public:
#if defined(__SSE__)
        alignas((N == 4 && M == 4) ? 16 : alignof(T))
#endif
        T m[N * M]{0};

        Matrix()
        {
        }

        template<typename ...A>
        Matrix(A... args):
            m{static_cast<T>(args)...}
        {
        }

        explicit Matrix(const T* array)
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

        static inline constexpr Matrix identity()
        {
            return Matrix(1, 0, 0, 0,
                          0, 1, 0, 0,
                          0, 0, 1, 0,
                          0, 0, 0, 1);
        }

        static void createLookAt(const Vector<3, T>& eyePosition,
                                 const Vector<3, T>& targetPosition,
                                 const Vector<3, T>& up, Matrix& dst)
        {
            createLookAt(eyePosition.v[0], eyePosition.v[1], eyePosition.v[2],
                         targetPosition.v[0], targetPosition.v[1], targetPosition.v[2],
                         up.v[0], up.v[1], up.v[2], dst);
        }

        static void createLookAt(T eyePositionX, T eyePositionY, T eyePositionZ,
                                 T targetPositionX, T targetPositionY, T targetPositionZ,
                                 T upX, T upY, T upZ, Matrix& dst)
        {
            Vector<3, T> eye(eyePositionX, eyePositionY, eyePositionZ);
            Vector<3, T> target(targetPositionX, targetPositionY, targetPositionZ);
            Vector<3, T> up(upX, upY, upZ);
            up.normalize();

            Vector<3, T> zaxis = target - eye;
            zaxis.normalize();

            Vector<3, T> xaxis = up.cross(zaxis);
            xaxis.normalize();

            Vector<3, T> yaxis = zaxis.cross(xaxis);
            yaxis.normalize();

            dst.m[0] = xaxis.v[0];
            dst.m[1] = yaxis.v[0];
            dst.m[2] = zaxis.v[0];
            dst.m[3] = 0;

            dst.m[4] = xaxis.v[1];
            dst.m[5] = yaxis.v[1];
            dst.m[6] = zaxis.v[1];
            dst.m[7] = 0;

            dst.m[8] = xaxis.v[2];
            dst.m[9] = yaxis.v[2];
            dst.m[10] = zaxis.v[2];
            dst.m[11] = 0;

            dst.m[12] = xaxis.dot(-eye);
            dst.m[13] = yaxis.dot(-eye);
            dst.m[14] = zaxis.dot(-eye);
            dst.m[15] = 1;
        }

        static void createPerspective(T fieldOfView, T aspectRatio, T zNearPlane, T zFarPlane, Matrix& dst)
        {
            assert(zFarPlane != zNearPlane);

            T theta = fieldOfView / 2;
            if (fabs(fmod(theta, PI / 2)) <= std::numeric_limits<T>::min())
                return;

            T divisor = tan(theta);
            assert(divisor);
            T factor = 1 / divisor;

            dst.setZero();

            assert(aspectRatio);
            dst.m[0] = (1 / aspectRatio) * factor;
            dst.m[5] = factor;
            dst.m[10] = zFarPlane / (zFarPlane - zNearPlane);
            dst.m[11] = 1;
            dst.m[14] = -zNearPlane * zFarPlane / (zFarPlane - zNearPlane);
        }

        static void createOrthographicFromSize(T width, T height, T zNearPlane, T zFarPlane, Matrix& dst)
        {
            T halfWidth = width / 2;
            T halfHeight = height / 2;
            createOrthographicOffCenter(-halfWidth, halfWidth,
                                        -halfHeight, halfHeight,
                                        zNearPlane, zFarPlane, dst);
        }

        static void createOrthographicOffCenter(T left, T right, T bottom, T top,
                                                T zNearPlane, T zFarPlane, Matrix& dst)
        {
            assert(right != left);
            assert(top != bottom);
            assert(zFarPlane != zNearPlane);

            dst.setZero();

            dst.m[0] = 2 / (right - left);
            dst.m[5] = 2 / (top - bottom);
            dst.m[10] = 1 / (zFarPlane - zNearPlane);
            dst.m[12] = (left + right) / (left - right);
            dst.m[13] = (bottom + top) / (bottom - top);
            dst.m[14] = zNearPlane / (zNearPlane - zFarPlane);
            dst.m[15] = 1;
        }

        static void createScale(const Vector<3, T>& scale, Matrix& dst)
        {
            dst.setIdentity();

            dst.m[0] = scale.v[0];
            dst.m[5] = scale.v[1];
            dst.m[10] = scale.v[2];
        }

        static void createScale(T xScale, T yScale, T zScale, Matrix& dst)
        {
            dst.setIdentity();

            dst.m[0] = xScale;
            dst.m[5] = yScale;
            dst.m[10] = zScale;
        }

        static void createRotation(const Vector<3, T>& axis, T angle, Matrix& dst)
        {
            T x = axis.v[0];
            T y = axis.v[1];
            T z = axis.v[2];

            // Make sure the input axis is normalized
            T n = x * x + y * y + z * z;
            if (n != 1)
            {
                // Not normalized
                n = sqrt(n);
                // Prevent divide too close to zero
                if (n > std::numeric_limits<T>::min())
                {
                    n = 1 / n;
                    x *= n;
                    y *= n;
                    z *= n;
                }
            }

            T c = cos(angle);
            T s = sin(angle);

            T t = 1 - c;
            T tx = t * x;
            T ty = t * y;
            T tz = t * z;
            T txy = tx * y;
            T txz = tx * z;
            T tyz = ty * z;
            T sx = s * x;
            T sy = s * y;
            T sz = s * z;

            dst.m[0] = c + tx * x;
            dst.m[4] = txy - sz;
            dst.m[8] = txz + sy;
            dst.m[12] = 0;

            dst.m[1] = txy + sz;
            dst.m[5] = c + ty * y;
            dst.m[9] = tyz - sx;
            dst.m[13] = 0;

            dst.m[2] = txz - sy;
            dst.m[6] = tyz + sx;
            dst.m[10] = c + tz * z;
            dst.m[14] = 0;

            dst.m[3] = 0;
            dst.m[7] = 0;
            dst.m[11] = 0;
            dst.m[15] = 1;
        }

        static void createRotationX(T angle, Matrix& dst)
        {
            dst.setIdentity();

            T c = cos(angle);
            T s = sin(angle);

            dst.m[5] = c;
            dst.m[9] = -s;
            dst.m[6] = s;
            dst.m[10] = c;
        }

        static void createRotationY(T angle, Matrix& dst)
        {
            dst.setIdentity();

            T c = cos(angle);
            T s = sin(angle);

            dst.m[0] = c;
            dst.m[8] = s;
            dst.m[2] = -s;
            dst.m[10] = c;
        }

        static void createRotationZ(T angle, Matrix& dst)
        {
            dst.setIdentity();

            T c = cos(angle);
            T s = sin(angle);

            dst.m[0] = c;
            dst.m[4] = -s;
            dst.m[1] = s;
            dst.m[5] = c;
        }

        static void createTranslation(const Vector<3, T>& translation, Matrix& dst)
        {
            dst.setIdentity();

            dst.m[12] = translation.v[0];
            dst.m[13] = translation.v[1];
            dst.m[14] = translation.v[2];
        }

        static void createTranslation(T xTranslation, T yTranslation, T zTranslation, Matrix& dst)
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

        void add(T scalar, Matrix& dst);

        void add(const Matrix& matrix)
        {
            add(*this, matrix, *this);
        }

        static void add(const Matrix& m1, const Matrix& m2, Matrix& dst);

        T determinant() const
        {
            T a0 = m[0] * m[5] - m[1] * m[4];
            T a1 = m[0] * m[6] - m[2] * m[4];
            T a2 = m[0] * m[7] - m[3] * m[4];
            T a3 = m[1] * m[6] - m[2] * m[5];
            T a4 = m[1] * m[7] - m[3] * m[5];
            T a5 = m[2] * m[7] - m[3] * m[6];
            T b0 = m[8] * m[13] - m[9] * m[12];
            T b1 = m[8] * m[14] - m[10] * m[12];
            T b2 = m[8] * m[15] - m[11] * m[12];
            T b3 = m[9] * m[14] - m[10] * m[13];
            T b4 = m[9] * m[15] - m[11] * m[13];
            T b5 = m[10] * m[15] - m[11] * m[14];

            // Calculate the determinant
            return a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
        }

        inline void getUpVector(Vector<3, T>& dst) const
        {
            dst.v[0] = m[4];
            dst.v[1] = m[5];
            dst.v[2] = m[6];
        }

        inline void getDownVector(Vector<3, T>& dst) const
        {
            dst.v[0] = -m[4];
            dst.v[1] = -m[5];
            dst.v[2] = -m[6];
        }

        inline void getLeftVector(Vector<3, T>& dst) const
        {
            dst.v[0] = -m[0];
            dst.v[1] = -m[1];
            dst.v[2] = -m[2];
        }

        inline void getRightVector(Vector<3, T>& dst) const
        {
            dst.v[0] = m[0];
            dst.v[1] = m[1];
            dst.v[2] = m[2];
        }

        inline void getForwardVector(Vector<3, T>& dst) const
        {
            dst.v[0] = -m[8];
            dst.v[1] = -m[9];
            dst.v[2] = -m[10];
        }

        inline void getBackVector(Vector<3, T>& dst) const
        {
            dst.v[0] = m[8];
            dst.v[1] = m[9];
            dst.v[2] = m[10];
        }

        void invert()
        {
            invert(*this);
        }

        void invert(Matrix& dst) const;

        inline bool isIdentity() const
        {
            return m[0] == 1 && m[1] == 0 && m[2] == 0 && m[3] == 0 &&
                   m[4] == 0 && m[5] == 1 && m[6] == 0 && m[7] == 0 &&
                   m[8] == 0 && m[9] == 0 && m[10] == 1 && m[11] == 0 &&
                   m[12] == 0 && m[13] == 0 && m[14] == 0 && m[15] == 1;
        }

        void multiply(T scalar)
        {
            multiply(scalar, *this);
        }

        void multiply(T scalar, Matrix& dst) const
        {
            multiply(*this, scalar, dst);
        }

        static void multiply(const Matrix& m, T scalar, Matrix& dst);

        void multiply(const Matrix& matrix)
        {
            multiply(*this, matrix, *this);
        }

        static void multiply(const Matrix& m1, const Matrix& m2, Matrix& dst);

        void negate()
        {
            negate(*this);
        }

        void negate(Matrix& dst) const;

        void rotate(const Vector<3, T>& axis, T angle)
        {
            rotate(axis, angle, *this);
        }

        void rotate(const Vector<3, T>& axis, T angle, Matrix& dst) const
        {
            Matrix r;
            createRotation(axis, angle, r);
            multiply(*this, r, dst);
        }

        void rotateX(T angle)
        {
            rotateX(angle, *this);
        }

        void rotateX(T angle, Matrix& dst) const
        {
            Matrix r;
            createRotationX(angle, r);
            multiply(*this, r, dst);
        }

        void rotateY(T angle)
        {
            rotateY(angle, *this);
        }

        void rotateY(T angle, Matrix& dst) const
        {
            Matrix r;
            createRotationY(angle, r);
            multiply(*this, r, dst);
        }

        void rotateZ(T angle)
        {
            rotateZ(angle, *this);
        }

        void rotateZ(T angle, Matrix& dst) const
        {
            Matrix r;
            createRotationZ(angle, r);
            multiply(*this, r, dst);
        }

        void scale(T value)
        {
            scale(value, *this);
        }

        void scale(T value, Matrix& dst) const
        {
            scale(value, value, value, dst);
        }

        void scale(T xScale, T yScale, T zScale)
        {
            scale(xScale, yScale, zScale, *this);
        }

        void scale(T xScale, T yScale, T zScale, Matrix& dst) const
        {
            Matrix s;
            createScale(xScale, yScale, zScale, s);
            multiply(*this, s, dst);
        }

        void scale(const Vector<3, T>& s)
        {
            scale(s.v[0], s.v[1], s.v[2], *this);
        }

        void scale(const Vector<3, T>& s, Matrix& dst) const
        {
            scale(s.v[0], s.v[1], s.v[2], dst);
        }

        inline void setIdentity()
        {
            m[0] = 1; m[1] = 0; m[2] = 0; m[3] = 0;
            m[4] = 0; m[5] = 1; m[6] = 0; m[7] = 0;
            m[8] = 0; m[9] = 0; m[10] = 1; m[11] = 0;
            m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
        }

        inline void setZero()
        {
            std::fill(m, m + sizeof(m) / sizeof(T), static_cast<T>(0));
        }

        void subtract(const Matrix& matrix)
        {
            subtract(*this, matrix, *this);
        }

        static void subtract(const Matrix& m1, const Matrix& m2, Matrix& dst);

        void transformPoint(Vector<3, T>& point) const
        {
            transformVector(point.v[0], point.v[1], point.v[2], 1, point);
        }

        void transformPoint(const Vector<3, T>& point, Vector<3, T>& dst) const
        {
            transformVector(point.v[0], point.v[1], point.v[2], 1, dst);
        }

        void transformVector(Vector<3, T>& vector) const
        {
            Vector<4, T> t;
            transformVector(Vector<4, T>(vector.v[0], vector.v[1], vector.v[2], static_cast<T>(0)), t);
            vector = Vector<3, T>(t.v[0], t.v[1], t.v[2]);
        }

        void transformVector(const Vector<3, T>& vector, Vector<3, T>& dst) const
        {
            transformVector(vector.v[0], vector.v[1], vector.v[2], 0, dst);
        }

        void transformVector(T x, T y, T z, T w, Vector<3, T>& dst) const
        {
            Vector<4, T> t;
            transformVector(Vector<4, T>(x, y, z, w), t);
            dst = Vector<3, T>(t.v[0], t.v[1], t.v[2]);
        }

        void transformVector(Vector<4, T>& vector) const
        {
            transformVector(vector, vector);
        }

        void transformVector(const Vector<4, T>& vector, Vector<4, T>& dst) const;

        void translate(T x, T y, T z)
        {
            translate(x, y, z, *this);
        }

        void translate(T x, T y, T z, Matrix& dst) const
        {
            Matrix t;
            createTranslation(x, y, z, t);
            multiply(*this, t, dst);
        }

        void translate(const Vector<3, T>& t)
        {
            translate(t.v[0], t.v[1], t.v[2], *this);
        }

        void translate(const Vector<3, T>& t, Matrix& dst) const
        {
            translate(t.v[0], t.v[1], t.v[2], dst);
        }

        void transpose()
        {
            transpose(*this);
        }

        void transpose(Matrix& dst) const;

        Vector<3, T> getTranslation() const
        {
            return Vector<3, T>(m[12], m[13], m[14]);
        }

        Vector<3, T> getScale() const
        {
            Vector<3, T> scale;
            scale.v[0] = Vector<3, T>(m[0], m[1], m[2]).length();
            scale.v[1] = Vector<3, T>(m[4], m[5], m[6]).length();
            scale.v[2] = Vector<3, T>(m[8], m[9], m[10]).length();

            return scale;
        }

        Quaternion<T> getRotation() const
        {
            Vector<3, T> scale = getScale();

            T m11 = m[0] / scale.v[0];
            T m21 = m[1] / scale.v[0];
            T m31 = m[2] / scale.v[0];

            T m12 = m[4] / scale.v[1];
            T m22 = m[5] / scale.v[1];
            T m32 = m[6] / scale.v[1];

            T m13 = m[8] / scale.v[2];
            T m23 = m[9] / scale.v[2];
            T m33 = m[10] / scale.v[2];

            Quaternion<T> result;
            result.v[0] = sqrt(std::max(static_cast<T>(0), 1 + m11 - m22 - m33)) / 2;
            result.v[1] = sqrt(std::max(static_cast<T>(0), 1 - m11 + m22 - m33)) / 2;
            result.v[2] = sqrt(std::max(static_cast<T>(0), 1 - m11 - m22 + m33)) / 2;
            result.v[3] = sqrt(std::max(static_cast<T>(0), 1 + m11 + m22 + m33)) / 2;

            result.v[0] *= sgn(result.v[0] * (m32 - m23));
            result.v[1] *= sgn(result.v[1] * (m13 - m31));
            result.v[2] *= sgn(result.v[2] * (m21 - m12));

            result.normalize();

            return result;
        }

        void setRotation(const Quaternion<T>& rotation)
        {
            T wx = rotation.v[3] * rotation.v[0];
            T wy = rotation.v[3] * rotation.v[1];
            T wz = rotation.v[3] * rotation.v[2];

            T xx = rotation.v[0] * rotation.v[0];
            T xy = rotation.v[0] * rotation.v[1];
            T xz = rotation.v[0] * rotation.v[2];

            T yy = rotation.v[1] * rotation.v[1];
            T yz = rotation.v[1] * rotation.v[2];

            T zz = rotation.v[2] * rotation.v[2];

            m[0] = 1 - 2 * (yy + zz);
            m[4] = 2 * (xy - wz);
            m[8] = 2 * (xz + wy);
            m[12] = 0;

            m[1] = 2 * (xy + wz);
            m[5] = 1 - 2 * (xx + zz);
            m[9] = 2 * (yz - wx);
            m[13] = 0;

            m[2] = 2 * (xz - wy);
            m[6] = 2 * (yz + wx);
            m[10] = 1 - 2 * (xx + yy);
            m[14] = 0;

            m[3] = 0;
            m[7] = 0;
            m[11] = 0;
            m[15] = 1;
        }

        inline const Matrix operator+(const Matrix& matrix) const
        {
            Matrix result(*this);
            add(result, matrix, result);
            return result;
        }

        inline Matrix& operator+=(const Matrix& matrix)
        {
            add(matrix);
            return *this;
        }

        inline const Matrix operator-(const Matrix& matrix) const
        {
            Matrix result(*this);
            subtract(result, matrix, result);
            return result;
        }

        inline Matrix& operator-=(const Matrix& matrix)
        {
            subtract(matrix);
            return *this;
        }

        inline const Matrix operator-() const
        {
            Matrix result(*this);
            negate(result);
            return result;
        }

        inline const Matrix operator*(const Matrix& matrix) const
        {
            Matrix result(*this);
            multiply(result, matrix, result);
            return result;
        }

        inline Matrix& operator*=(const Matrix& matrix)
        {
            multiply(matrix);
            return *this;
        }

        inline bool operator==(const Matrix& matrix)
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

        inline bool operator!=(const Matrix& matrix)
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

        inline const Vector<3, T> operator*(const Vector<3, T>& v)
        {
            Vector<3, T> x;
            transformVector(v, x);
            return x;
        }

        inline const Vector<4, T> operator*(const Vector<4, T>& v)
        {
            Vector<4, T> x;
            transformVector(v, x);
            return x;
        }
    };

    template<class T>
    inline Vector<3, T>& operator*=(Vector<3, T>& v, const Matrix<4, 4, T>& m)
    {
        m.transformVector(v);
        return v;
    }

    template<class T>
    inline Vector<4, T>& operator*=(Vector<4, T>& v, const Matrix<4, 4, T>& m)
    {
        m.transformVector(v);
        return v;
    }

    using Matrix4F = Matrix<4, 4, float>;
}

#endif // OUZEL_MATH_MATRIX_HPP
