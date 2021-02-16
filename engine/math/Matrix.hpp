// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_MATRIX_HPP
#define OUZEL_MATH_MATRIX_HPP

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>
#include "Constants.hpp"
#include "ConvexVolume.hpp"
#include "Plane.hpp"
#include "Quaternion.hpp"
#include "Vector.hpp"

namespace ouzel
{
    template <typename T, std::size_t C, std::size_t R = C> class Matrix final
    {
    public:
#if defined(__SSE__)
        alignas((C == 4 && R == 4) ? 4 * sizeof(T) : alignof(T))
#endif
        std::array<T, C * R> m{}; // row-major matrix (transformation is pre-multiplying)

        constexpr Matrix() noexcept {}

        template <typename ...A>
        explicit constexpr Matrix(const A... args) noexcept:
            m{args...}
        {
        }

        auto operator[](const std::size_t row) noexcept { return &m[row * C]; }
        constexpr auto operator[](const std::size_t row) const noexcept { return &m[row * C]; }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == Y)>* = nullptr>
        static constexpr Matrix identity() noexcept
        {
            return generateIdentity(std::make_index_sequence<C * R>{});
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void setLookAt(const Vector<T, 3>& eyePosition,
                       const Vector<T, 3>& targetPosition,
                       const Vector<T, 3>& up) noexcept
        {
            setLookAt(eyePosition.v[0], eyePosition.v[1], eyePosition.v[2],
                      targetPosition.v[0], targetPosition.v[1], targetPosition.v[2],
                      up.v[0], up.v[1], up.v[2]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void setLookAt(const T eyePositionX, const T eyePositionY, const T eyePositionZ,
                       const T targetPositionX, const T targetPositionY, const T targetPositionZ,
                       const T upX, const T upY, const T upZ) noexcept
        {
            const Vector<T, 3> eye(eyePositionX, eyePositionY, eyePositionZ);
            const Vector<T, 3> target(targetPositionX, targetPositionY, targetPositionZ);
            Vector<T, 3> up(upX, upY, upZ);
            up.normalize();

            Vector<T, 3> zaxis = target - eye;
            zaxis.normalize();

            Vector<T, 3> xaxis = up.cross(zaxis);
            xaxis.normalize();

            Vector<T, 3> yaxis = zaxis.cross(xaxis);
            yaxis.normalize();

            m[0] = xaxis.v[0];
            m[1] = yaxis.v[0];
            m[2] = zaxis.v[0];
            m[3] = T(0);

            m[4] = xaxis.v[1];
            m[5] = yaxis.v[1];
            m[6] = zaxis.v[1];
            m[7] = T(0);

            m[8] = xaxis.v[2];
            m[9] = yaxis.v[2];
            m[10] = zaxis.v[2];
            m[11] = T(0);

            m[12] = xaxis.dot(-eye);
            m[13] = yaxis.dot(-eye);
            m[14] = zaxis.dot(-eye);
            m[15] = T(1);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void setPerspective(const T fieldOfView, const T aspectRatio,
                            const T nearClip, const T farClip) noexcept
        {
            assert(aspectRatio);
            assert(farClip != nearClip);

            const T theta = fieldOfView / T(2);
            if (std::fabs(std::fmod(theta, pi<T> / T(2))) <= std::numeric_limits<T>::min())
                return;

            const T divisor = std::tan(theta);
            assert(divisor);
            const T factor = T(1) / divisor;

            setZero();

            m[0] = factor / aspectRatio;
            m[5] = factor;
            m[10] = farClip / (farClip - nearClip);
            m[11] = T(1);
            m[14] = -nearClip * farClip / (farClip - nearClip);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void setOrthographic(const T width, const T height,
                             const T nearClip, const T farClip) noexcept
        {
            assert(width);
            assert(height);
            assert(farClip != nearClip);

            setZero();

            m[0] = T(2) / width;
            m[5] = T(2) / height;
            m[10] = T(1) / (farClip - nearClip);
            m[14] = nearClip / (nearClip - farClip);
            m[15] = T(1);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void setOrthographic(const T left, const T right,
                             const T bottom, const T top,
                             const T near, const T far) noexcept
        {
            assert(right != left);
            assert(top != bottom);
            assert(far != near);

            setZero();

            m[0] = T(2) / (right - left);
            m[5] = T(2) / (top - bottom);
            m[10] = T(1) / (far - near);
            m[12] = (left + right) / (left - right);
            m[13] = (bottom + top) / (bottom - top);
            m[14] = near / (near - far);
            m[15] = T(1);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == Y)>* = nullptr>
        void setScale(const Vector<T, R - 1>& scale) noexcept
        {
            setIdentity();

            for (std::size_t i = 0; i < R - 1; ++i)
                m[i * C + i] = scale[i];
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 3 && Y == 3)>* = nullptr>
        void setScale(const T x, const T y) noexcept
        {
            setIdentity();

            m[0] = x;
            m[4] = y;
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void setScale(const T x, const T y, const T z) noexcept
        {
            setIdentity();

            m[0] = x;
            m[5] = y;
            m[10] = z;
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 3 && Y == 3)>* = nullptr>
        void setRotation(const T angle) noexcept
        {
            setIdentity();

            const T c = std::cos(angle);
            const T s = std::sin(angle);

            m[0] = c;
            m[3] = -s;
            m[1] = s;
            m[4] = c;
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void setRotation(const Vector<T, 3>& axis, T angle) noexcept
        {
            T x = axis.v[0];
            T y = axis.v[1];
            T z = axis.v[2];

            const T squared = x * x + y * y + z * z;
            if (squared != T(1))
            {
                const T length = std::sqrt(squared);
                if (length > std::numeric_limits<T>::min())
                {
                    const T multiplier = T(1) / length;
                    x *= multiplier;
                    y *= multiplier;
                    z *= multiplier;
                }
            }

            const T c = std::cos(angle);
            const T s = std::sin(angle);

            const T t = T(1) - c;
            const T tx = t * x;
            const T ty = t * y;
            const T tz = t * z;
            const T txy = tx * y;
            const T txz = tx * z;
            const T tyz = ty * z;
            const T sx = s * x;
            const T sy = s * y;
            const T sz = s * z;

            m[0] = c + tx * x;
            m[4] = txy - sz;
            m[8] = txz + sy;
            m[12] = T(0);

            m[1] = txy + sz;
            m[5] = c + ty * y;
            m[9] = tyz - sx;
            m[13] = T(0);

            m[2] = txz - sy;
            m[6] = tyz + sx;
            m[10] = c + tz * z;
            m[14] = T(0);

            m[3] = T(0);
            m[7] = T(0);
            m[11] = T(0);
            m[15] = T(1);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void setRotation(const Quaternion<T>& rotation) noexcept
        {
            const T wx = rotation.v[3] * rotation.v[0];
            const T wy = rotation.v[3] * rotation.v[1];
            const T wz = rotation.v[3] * rotation.v[2];

            const T xx = rotation.v[0] * rotation.v[0];
            const T xy = rotation.v[0] * rotation.v[1];
            const T xz = rotation.v[0] * rotation.v[2];

            const T yy = rotation.v[1] * rotation.v[1];
            const T yz = rotation.v[1] * rotation.v[2];

            const T zz = rotation.v[2] * rotation.v[2];

            m[0] = T(1) - T(2) * (yy + zz);
            m[4] = T(2) * (xy - wz);
            m[8] = T(2) * (xz + wy);
            m[12] = T(0);

            m[1] = T(2) * (xy + wz);
            m[5] = T(1) - T(2) * (xx + zz);
            m[9] = T(2) * (yz - wx);
            m[13] = T(0);

            m[2] = T(2) * (xz - wy);
            m[6] = T(2) * (yz + wx);
            m[10] = T(1) - T(2) * (xx + yy);
            m[14] = T(0);

            m[3] = T(0);
            m[7] = T(0);
            m[11] = T(0);
            m[15] = T(1);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void setRotationX(const T angle) noexcept
        {
            setIdentity();

            const T c = std::cos(angle);
            const T s = std::sin(angle);

            m[5] = c;
            m[9] = -s;
            m[6] = s;
            m[10] = c;
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void setRotationY(const T angle) noexcept
        {
            setIdentity();

            const T c = std::cos(angle);
            const T s = std::sin(angle);

            m[0] = c;
            m[8] = s;
            m[2] = -s;
            m[10] = c;
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void setRotationZ(const T angle) noexcept
        {
            setIdentity();

            const T c = std::cos(angle);
            const T s = std::sin(angle);

            m[0] = c;
            m[4] = -s;
            m[1] = s;
            m[5] = c;
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == Y)>* = nullptr>
        void setTranslation(const Vector<T, R - 1>& translation) noexcept
        {
            setIdentity();

            for (std::size_t i = 0; i < R - 1; ++i)
                m[(R - 1) * C + i] = translation[i];
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 3 && Y == 3)>* = nullptr>
        void setTranslation(const T x, const T y) noexcept
        {
            setIdentity();

            m[6] = x;
            m[7] = y;
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void setTranslation(const T x, const T y, const T z) noexcept
        {
            setIdentity();

            m[12] = x;
            m[13] = y;
            m[14] = z;
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        auto getFrustumLeftPlane() const noexcept
        {
            return Plane<T>::makeFrustumPlane(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        auto getFrustumRightPlane() const noexcept
        {
            return Plane<T>::makeFrustumPlane(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        auto getFrustumBottomPlane() const noexcept
        {
            return Plane<T>::makeFrustumPlane(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        auto getFrustumTopPlane() const noexcept
        {
            return Plane<T>::makeFrustumPlane(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        auto getFrustumNearPlane() const noexcept
        {
            return Plane<T>::makeFrustumPlane(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        auto getFrustumFarPlane() const noexcept
        {
            return Plane<T>::makeFrustumPlane(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        auto getFrustum() const noexcept
        {
            return ConvexVolume<T>({
                getFrustumLeftPlane(),
                getFrustumRightPlane(),
                getFrustumBottomPlane(),
                getFrustumTopPlane(),
                getFrustumNearPlane(),
                getFrustumFarPlane()
            });
        }

        void add(const T scalar) noexcept
        {
            add(scalar, *this);
        }

        void add(const T scalar, Matrix& dst) const noexcept
        {
            for (std::size_t i = 0; i < C * R; ++i)
                dst.m[i] = m[i] + scalar;
        }

        void add(const Matrix& matrix) noexcept
        {
            add(matrix, *this);
        }

        void add(const Matrix& matrix, Matrix& dst) const noexcept
        {
            for (std::size_t i = 0; i < C * R; ++i)
                dst.m[i] = m[i] + matrix.m[i];
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 1 && Y == 1)>* = nullptr>
        constexpr T getDeterminant() const noexcept
        {
            return m[0];
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 2 && Y == 2)>* = nullptr>
        constexpr T getDeterminant() const noexcept
        {
            return m[0] * m[3] - m[1] * m[2];
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 3 && Y == 3)>* = nullptr>
        constexpr T getDeterminant() const noexcept
        {
            constexpr T a0 = m[0] * (m[4] * m[8] - m[5] * m[7]);
            constexpr T a1 = m[1] * (m[3] * m[8] - m[5] * m[6]);
            constexpr T a2 = m[2] * (m[3] * m[7] - m[4] * m[6]);
            return a0 - a1 + a2;
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        constexpr T getDeterminant() const noexcept
        {
            constexpr T a0 = m[0] * m[5] - m[1] * m[4];
            constexpr T a1 = m[0] * m[6] - m[2] * m[4];
            constexpr T a2 = m[0] * m[7] - m[3] * m[4];
            constexpr T a3 = m[1] * m[6] - m[2] * m[5];
            constexpr T a4 = m[1] * m[7] - m[3] * m[5];
            constexpr T a5 = m[2] * m[7] - m[3] * m[6];
            constexpr T b0 = m[8] * m[13] - m[9] * m[12];
            constexpr T b1 = m[8] * m[14] - m[10] * m[12];
            constexpr T b2 = m[8] * m[15] - m[11] * m[12];
            constexpr T b3 = m[9] * m[14] - m[10] * m[13];
            constexpr T b4 = m[9] * m[15] - m[11] * m[13];
            constexpr T b5 = m[10] * m[15] - m[11] * m[14];
            return a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        auto getUpVector() const noexcept
        {
            return Vector<T, 3>(m[4], m[5], m[6]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        auto getDownVector() const noexcept
        {
            return Vector<T, 3>(-m[4], -m[5], -m[6]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        auto getLeftVector() const noexcept
        {
            return Vector<T, 3>(-m[0], -m[1], -m[2]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        auto getRightVector() const noexcept
        {
            return Vector<T, 3>(m[0], m[1], m[2]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        auto getForwardVector() const noexcept
        {
            return Vector<T, 3>(-m[8], -m[9], -m[10]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        auto getBackVector() const noexcept
        {
            return Vector<T, 3>(m[8], m[9], m[10]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 1 && Y == 1)>* = nullptr>
        void invert() noexcept
        {
            const T determinant = m[0];

            // Close to zero, can't invert
            if (std::fabs(determinant) <= std::numeric_limits<T>::min()) return;

            m[0] = T(1) / m[0];
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 2 && Y == 2)>* = nullptr>
        void invert() noexcept
        {
            const T determinant = m[0] * m[3] - m[1] * m[2];

            // Close to zero, can't invert
            if (std::fabs(determinant) <= std::numeric_limits<T>::min()) return;

            Matrix adjugate;
            adjugate.m[0] = m[3];
            adjugate.m[1] = -m[1];
            adjugate.m[2] = -m[2];
            adjugate.m[3] = m[0];

            adjugate.multiply(T(1) / determinant, *this);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 3 && Y == 3)>* = nullptr>
        void invert() noexcept
        {
            const T a0 = m[0] * (m[4] * m[8] - m[5] * m[7]);
            const T a1 = m[1] * (m[3] * m[8] - m[5] * m[6]);
            const T a2 = m[2] * (m[3] * m[7] - m[4] * m[6]);

            const T determinant = a0 - a1 + a2;

            // Close to zero, can't invert
            if (std::fabs(determinant) <= std::numeric_limits<T>::min()) return;

            Matrix inverse;
            inverse.m[0] = m[4] * m[8] - m[5] * m[7];
            inverse.m[1] = -(m[3] * m[8] - m[5] * m[6]);
            inverse.m[2] = m[3] * m[7] - m[4] * m[6];
            inverse.m[3] = -(m[1] * m[8] - m[2] * m[7]);
            inverse.m[4] = m[0] * m[8] - m[2] * m[6];
            inverse.m[5] = -(m[0] * m[7] - m[1] * m[6]);
            inverse.m[6] = m[1] * m[5] - m[2] * m[4];
            inverse.m[7] = -(m[0] * m[5] - m[2] * m[3]);
            inverse.m[8] = m[0] * m[4] - m[1] * m[3];

            inverse.multiply(T(1) / determinant, *this);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void invert() noexcept
        {
            const T a0 = m[0] * m[5] - m[1] * m[4];
            const T a1 = m[0] * m[6] - m[2] * m[4];
            const T a2 = m[0] * m[7] - m[3] * m[4];
            const T a3 = m[1] * m[6] - m[2] * m[5];
            const T a4 = m[1] * m[7] - m[3] * m[5];
            const T a5 = m[2] * m[7] - m[3] * m[6];
            const T b0 = m[8] * m[13] - m[9] * m[12];
            const T b1 = m[8] * m[14] - m[10] * m[12];
            const T b2 = m[8] * m[15] - m[11] * m[12];
            const T b3 = m[9] * m[14] - m[10] * m[13];
            const T b4 = m[9] * m[15] - m[11] * m[13];
            const T b5 = m[10] * m[15] - m[11] * m[14];

            const T determinant = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

            // Close to zero, can't invert
            if (std::fabs(determinant) <= std::numeric_limits<T>::min()) return;

            Matrix adjugate;
            adjugate.m[0] = m[5] * b5 - m[6] * b4 + m[7] * b3;
            adjugate.m[1] = -m[1] * b5 + m[2] * b4 - m[3] * b3;
            adjugate.m[2] = m[13] * a5 - m[14] * a4 + m[15] * a3;
            adjugate.m[3] = -m[9] * a5 + m[10] * a4 - m[11] * a3;

            adjugate.m[4] = -m[4] * b5 + m[6] * b2 - m[7] * b1;
            adjugate.m[5] = m[0] * b5 - m[2] * b2 + m[3] * b1;
            adjugate.m[6] = -m[12] * a5 + m[14] * a2 - m[15] * a1;
            adjugate.m[7] = m[8] * a5 - m[10] * a2 + m[11] * a1;

            adjugate.m[8] = m[4] * b4 - m[5] * b2 + m[7] * b0;
            adjugate.m[9] = -m[0] * b4 + m[1] * b2 - m[3] * b0;
            adjugate.m[10] = m[12] * a4 - m[13] * a2 + m[15] * a0;
            adjugate.m[11] = -m[8] * a4 + m[9] * a2 - m[11] * a0;

            adjugate.m[12] = -m[4] * b3 + m[5] * b1 - m[6] * b0;
            adjugate.m[13] = m[0] * b3 - m[1] * b1 + m[2] * b0;
            adjugate.m[14] = -m[12] * a3 + m[13] * a1 - m[14] * a0;
            adjugate.m[15] = m[8] * a3 - m[9] * a1 + m[10] * a0;

            adjugate.multiply(T(1) / determinant, *this);
        }

        constexpr auto isIdentity() const noexcept
        {
            if constexpr (C != R) return false;

            for (std::size_t r = 0; r < R; ++r)
                for (std::size_t c = 0; c < C; ++c)
                    if (m[r * C + c] != (r == c ? T(1) : T(0)))
                        return false;
            return true;
        }

        void multiply(const T scalar) noexcept
        {
            multiply(scalar, *this);
        }

        void multiply(const T scalar, Matrix& dst) const noexcept
        {
            for (std::size_t i = 0; i < C * R; ++i)
                dst.m[i] = m[i] * scalar;
        }

        void multiply(const Matrix& matrix) noexcept
        {
            multiply(matrix, *this);
        }

        void multiply(const Matrix& matrix, Matrix& dst) const noexcept
        {
            const std::array<T, 16> product = {
                m[0] * matrix.m[0] + m[4] * matrix.m[1] + m[8] * matrix.m[2] + m[12] * matrix.m[3],
                m[1] * matrix.m[0] + m[5] * matrix.m[1] + m[9] * matrix.m[2] + m[13] * matrix.m[3],
                m[2] * matrix.m[0] + m[6] * matrix.m[1] + m[10] * matrix.m[2] + m[14] * matrix.m[3],
                m[3] * matrix.m[0] + m[7] * matrix.m[1] + m[11] * matrix.m[2] + m[15] * matrix.m[3],

                m[0] * matrix.m[4] + m[4] * matrix.m[5] + m[8] * matrix.m[6] + m[12] * matrix.m[7],
                m[1] * matrix.m[4] + m[5] * matrix.m[5] + m[9] * matrix.m[6] + m[13] * matrix.m[7],
                m[2] * matrix.m[4] + m[6] * matrix.m[5] + m[10] * matrix.m[6] + m[14] * matrix.m[7],
                m[3] * matrix.m[4] + m[7] * matrix.m[5] + m[11] * matrix.m[6] + m[15] * matrix.m[7],

                m[0] * matrix.m[8] + m[4] * matrix.m[9] + m[8] * matrix.m[10] + m[12] * matrix.m[11],
                m[1] * matrix.m[8] + m[5] * matrix.m[9] + m[9] * matrix.m[10] + m[13] * matrix.m[11],
                m[2] * matrix.m[8] + m[6] * matrix.m[9] + m[10] * matrix.m[10] + m[14] * matrix.m[11],
                m[3] * matrix.m[8] + m[7] * matrix.m[9] + m[11] * matrix.m[10] + m[15] * matrix.m[11],

                m[0] * matrix.m[12] + m[4] * matrix.m[13] + m[8] * matrix.m[14] + m[12] * matrix.m[15],
                m[1] * matrix.m[12] + m[5] * matrix.m[13] + m[9] * matrix.m[14] + m[13] * matrix.m[15],
                m[2] * matrix.m[12] + m[6] * matrix.m[13] + m[10] * matrix.m[14] + m[14] * matrix.m[15],
                m[3] * matrix.m[12] + m[7] * matrix.m[13] + m[11] * matrix.m[14] + m[15] * matrix.m[15]
            };

            std::copy(product.begin(), product.end(), dst.m.begin());
        }

        void negate() noexcept
        {
            for (T& c : m)
                c = -c;
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == Y)>* = nullptr>
        void setIdentity() noexcept
        {
            for (std::size_t r = 0; r < R; ++r)
                for (std::size_t c = 0; c < C; ++c)
                    m[r * C + c] = static_cast<T>(r == c ? T(1) : T(0));
        }

        void setZero() noexcept
        {
            for (T& c : m)
                c = T(0);
        }

        void subtract(const T scalar) noexcept
        {
            subtract(scalar, *this);
        }

        void subtract(const T scalar, Matrix& dst) const noexcept
        {
            for (std::size_t i = 0; i < C * R; ++i)
                dst.m[i] = m[i] - scalar;
        }

        void subtract(const Matrix& matrix) noexcept
        {
            subtract(matrix, *this);
        }

        void subtract(const Matrix& matrix, Matrix& dst) const noexcept
        {
            for (std::size_t i = 0; i < C * R; ++i)
                dst.m[i] = m[i] - matrix.m[i];
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void transformPoint(Vector<T, 3>& point) const noexcept
        {
            transformVector(point.v[0], point.v[1], point.v[2], T(1), point);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void transformPoint(const Vector<T, 3>& point, Vector<T, 3>& dst) const noexcept
        {
            transformVector(point.v[0], point.v[1], point.v[2], T(1), dst);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void transformVector(Vector<T, 3>& v) const noexcept
        {
            Vector<T, 4> t;
            transformVector(Vector<T, 4>(v.v[0], v.v[1], v.v[2], T(0)), t);
            v = Vector<T, 3>(t.v[0], t.v[1], t.v[2]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void transformVector(const Vector<T, 3>& v, Vector<T, 3>& dst) const noexcept
        {
            transformVector(v.v[0], v.v[1], v.v[2], T(0), dst);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void transformVector(const T x, const T y, const T z, const T w,
                             Vector<T, 3>& dst) const noexcept
        {
            Vector<T, 4> t;
            transformVector(Vector<T, 4>(x, y, z, w), t);
            dst = Vector<T, 3>(t.v[0], t.v[1], t.v[2]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        void transformVector(Vector<T, 4>& v) const noexcept
        {
            transformVector(v, v);
        }

        void transformVector(const Vector<T, 4>& v, Vector<T, 4>& dst) const noexcept
        {
            assert(&v != &dst);
            dst.v[0] = v.v[0] * m[0] + v.v[1] * m[4] + v.v[2] * m[8] + v.v[3] * m[12];
            dst.v[1] = v.v[0] * m[1] + v.v[1] * m[5] + v.v[2] * m[9] + v.v[3] * m[13];
            dst.v[2] = v.v[0] * m[2] + v.v[1] * m[6] + v.v[2] * m[10] + v.v[3] * m[14];
            dst.v[3] = v.v[0] * m[3] + v.v[1] * m[7] + v.v[2] * m[11] + v.v[3] * m[15];
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == Y)>* = nullptr>
        void transpose() noexcept
        {
            transpose(*this);
        }

        void transpose(Matrix& dst) const noexcept
        {
            const std::array<T, C * R> t;

            for (std::size_t column = 0; column < C; ++column)
                for (std::size_t row = 0; row < C; ++row)
                    t[row * C + column] = t[column * R + row];

            std::copy(t.begin(), t.end(), dst.m.begin());
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 3 && Y == 3)>* = nullptr>
        constexpr Vector<T, 2> getTranslation() const noexcept
        {
            return Vector<T, 2>(m[6], m[7]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        constexpr Vector<T, 3> getTranslation() const noexcept
        {
            return Vector<T, 3>(m[12], m[13], m[14]);
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 3 && Y == 3)>* = nullptr>
        Vector<T, 2> getScale() const noexcept
        {
            Vector<T, 2> scale;
            scale.v[0] = Vector<T, 2>(m[0], m[1]).length();
            scale.v[1] = Vector<T, 2>(m[3], m[4]).length();

            return scale;
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        Vector<T, 3> getScale() const noexcept
        {
            Vector<T, 3> scale;
            scale.v[0] = Vector<T, 3>(m[0], m[1], m[2]).length();
            scale.v[1] = Vector<T, 3>(m[4], m[5], m[6]).length();
            scale.v[2] = Vector<T, 3>(m[8], m[9], m[10]).length();

            return scale;
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        auto getRotation() const noexcept
        {
            const Vector<T, 3> scale = getScale();

            const T m11 = m[0] / scale.v[0];
            const T m21 = m[1] / scale.v[0];
            const T m31 = m[2] / scale.v[0];

            const T m12 = m[4] / scale.v[1];
            const T m22 = m[5] / scale.v[1];
            const T m32 = m[6] / scale.v[1];

            const T m13 = m[8] / scale.v[2];
            const T m23 = m[9] / scale.v[2];
            const T m33 = m[10] / scale.v[2];

            Quaternion<T> result;
            result.v[0] = std::sqrt(std::max(T(0), T(1) + m11 - m22 - m33)) / T(2);
            result.v[1] = std::sqrt(std::max(T(0), T(1) - m11 + m22 - m33)) / T(2);
            result.v[2] = std::sqrt(std::max(T(0), T(1) - m11 - m22 + m33)) / T(2);
            result.v[3] = std::sqrt(std::max(T(0), T(1) + m11 + m22 + m33)) / T(2);

            // The problem with using copysign: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/paul.htm
            result.v[0] = std::copysign(result.v[0], m32 - m23);
            result.v[1] = std::copysign(result.v[1], m13 - m31);
            result.v[2] = std::copysign(result.v[2], m21 - m12);

            result.normalize();

            return result;
        }

        const Matrix operator+(const Matrix& matrix) const noexcept
        {
            Matrix result(*this);
            result.add(matrix);
            return result;
        }

        Matrix& operator+=(const Matrix& matrix) noexcept
        {
            add(matrix);
            return *this;
        }

        const Matrix operator-(const Matrix& matrix) const noexcept
        {
            Matrix result(*this);
            result.subtract(matrix);
            return result;
        }

        Matrix& operator-=(const Matrix& matrix) noexcept
        {
            subtract(matrix);
            return *this;
        }

        const Matrix operator-() const noexcept
        {
            Matrix result(*this);
            result.negate();
            return result;
        }

        const Matrix operator*(const Matrix& matrix) const noexcept
        {
            Matrix result(*this);
            result.multiply(matrix);
            return result;
        }

        Matrix& operator*=(const Matrix& matrix) noexcept
        {
            multiply(matrix);
            return *this;
        }

        const Matrix operator*(float scalar) const noexcept
        {
            Matrix result(*this);
            result.multiply(scalar);
            return result;
        }

        Matrix& operator*=(float scalar) noexcept
        {
            multiply(scalar);
            return *this;
        }

        constexpr bool operator==(const Matrix& matrix) const noexcept
        {
            for (std::size_t i = 0; i < C * R; ++i)
                if (m[i] != matrix.m[i])
                    return false;

            return true;
        }

        constexpr bool operator!=(const Matrix& matrix) const noexcept
        {
            for (std::size_t i = 0; i < C * R; ++i)
                if (m[i] != matrix.m[i])
                    return true;

            return false;
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        const Vector<T, 3> operator*(const Vector<T, 3>& v) const noexcept
        {
            Vector<T, 3> x;
            transformVector(v, x);
            return x;
        }

        template <auto X = C, auto Y = R, std::enable_if_t<(X == 4 && Y == 4)>* = nullptr>
        const Vector<T, 4> operator*(const Vector<T, 4>& v) const noexcept
        {
            Vector<T, 4> x;
            transformVector(v, x);
            return x;
        }

    private:
        template <std::size_t...I>
        static constexpr Matrix generateIdentity(std::index_sequence<I...>)
        {
            return Matrix{
                ((I % C == I / R) ? T(1) : T(0))...
            };
        }
    };

    template <typename T>
    Vector<T, 3>& operator*=(Vector<T, 3>& v, const Matrix<T, 4, 4>& m) noexcept
    {
        m.transformVector(v);
        return v;
    }

    template <typename T>
    Vector<T, 4>& operator*=(Vector<T, 4>& v, const Matrix<T, 4, 4>& m) noexcept
    {
        m.transformVector(v);
        return v;
    }

    template <typename T, std::size_t C, std::size_t R>
    const Matrix<T, C, R> operator*(const T scalar, const Matrix<T, C, R>& m) noexcept
    {
        return m * scalar;
    }

    using Matrix4F = Matrix<float, 4>;
}

#endif // OUZEL_MATH_MATRIX_HPP
