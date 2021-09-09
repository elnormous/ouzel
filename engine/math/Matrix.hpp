// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_MATRIX_HPP
#define OUZEL_MATH_MATRIX_HPP

#include <algorithm>
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
    template <typename T, std::size_t rows, std::size_t cols = rows> class Matrix final
    {
    public:
#ifdef __SSE__
        alignas((rows == 4 && cols == 4) ? 4 * sizeof(T) : alignof(T))
#endif
        std::array<T, cols * rows> m; // row-major matrix (transformation is pre-multiplying)

        [[nodiscard]] auto operator[](const std::size_t row) noexcept { return &m[row * cols]; }
        [[nodiscard]] constexpr auto operator[](const std::size_t row) const noexcept { return &m[row * cols]; }

        template <auto r = rows, auto c = cols, std::enable_if_t<(c == r)>* = nullptr>
        [[nodiscard]] static constexpr Matrix identity() noexcept
        {
            return generateIdentity(std::make_index_sequence<cols * rows>{});
        }

        void setLookAt(const Vector<T, 3>& eyePosition,
                       const Vector<T, 3>& targetPosition,
                       const Vector<T, 3>& up) noexcept
        {
            static_assert(rows == 4 && cols == 4);
            
            setLookAt(eyePosition.v[0], eyePosition.v[1], eyePosition.v[2],
                      targetPosition.v[0], targetPosition.v[1], targetPosition.v[2],
                      up.v[0], up.v[1], up.v[2]);
        }

        void setLookAt(const T eyePositionX, const T eyePositionY, const T eyePositionZ,
                       const T targetPositionX, const T targetPositionY, const T targetPositionZ,
                       const T upX, const T upY, const T upZ) noexcept
        {
            static_assert(rows == 4 && cols == 4);

            const Vector<T, 3> eye{eyePositionX, eyePositionY, eyePositionZ};
            const Vector<T, 3> target{targetPositionX, targetPositionY, targetPositionZ};
            Vector<T, 3> up{upX, upY, upZ};
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

        void setPerspective(const T fieldOfView, const T aspectRatio,
                            const T nearClip, const T farClip) noexcept
        {
            static_assert(rows == 4 && cols == 4);

            assert(aspectRatio);
            assert(farClip != nearClip);

            const auto theta = fieldOfView / T(2);
            if (std::fabs(std::fmod(theta, pi<T> / T(2))) <= std::numeric_limits<T>::epsilon())
                return;

            const auto divisor = std::tan(theta);
            assert(divisor);
            const auto factor = T(1) / divisor;

            setZero();

            m[0] = factor / aspectRatio;
            m[5] = factor;
            m[10] = farClip / (farClip - nearClip);
            m[11] = T(1);
            m[14] = -nearClip * farClip / (farClip - nearClip);
        }

        void setOrthographic(const T width, const T height,
                             const T nearClip, const T farClip) noexcept
        {
            static_assert(rows == 4 && cols == 4);

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

        void setOrthographic(const T left, const T right,
                             const T bottom, const T top,
                             const T nearClip, const T farClip) noexcept
        {
            static_assert(rows == 4 && cols == 4);

            assert(right != left);
            assert(top != bottom);
            assert(farClip != nearClip);

            setZero();

            m[0] = T(2) / (right - left);
            m[5] = T(2) / (top - bottom);
            m[10] = T(1) / (farClip - nearClip);
            m[12] = (left + right) / (left - right);
            m[13] = (bottom + top) / (bottom - top);
            m[14] = nearClip / (nearClip - farClip);
            m[15] = T(1);
        }

        void setScale(const Vector<T, rows - 1>& scale) noexcept
        {
            static_assert(cols == rows);

            setIdentity();

            for (std::size_t i = 0; i < rows - 1; ++i)
                m[i * cols + i] = scale[i];
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 3 && c == 3)>* = nullptr>
        void setRotation(const T angle) noexcept
        {
            setIdentity();

            const auto cosine = std::cos(angle);
            const auto sine = std::sin(angle);

            m[0] = cosine;
            m[3] = -sine;
            m[1] = sine;
            m[4] = cosine;
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 4 && c == 4)>* = nullptr>
        void setRotation(const Vector<T, 3>& axis, T angle) noexcept
        {
            const auto x = axis.v[0];
            const auto y = axis.v[1];
            const auto z = axis.v[2];

            const auto squared = x * x + y * y + z * z;
            if (squared != T(1))
            {
                const auto length = std::sqrt(squared);
                if (length > std::numeric_limits<T>::epsilon()) // bigger than zero
                {
                    x /= length;
                    y /= length;
                    z /= length;
                }
            }

            const auto cosine = std::cos(angle);
            const auto sine = std::sin(angle);

            const auto t = T(1) - cosine;
            const auto tx = t * x;
            const auto ty = t * y;
            const auto tz = t * z;
            const auto txy = tx * y;
            const auto txz = tx * z;
            const auto tyz = ty * z;
            const auto sx = sine * x;
            const auto sy = sine * y;
            const auto sz = sine * z;

            m[0] = cosine + tx * x;
            m[4] = txy - sz;
            m[8] = txz + sy;
            m[12] = T(0);

            m[1] = txy + sz;
            m[5] = cosine + ty * y;
            m[9] = tyz - sx;
            m[13] = T(0);

            m[2] = txz - sy;
            m[6] = tyz + sx;
            m[10] = cosine + tz * z;
            m[14] = T(0);

            m[3] = T(0);
            m[7] = T(0);
            m[11] = T(0);
            m[15] = T(1);
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 4 && c == 4)>* = nullptr>
        void setRotation(const Quaternion<T>& rotation) noexcept
        {
            const auto wx = rotation.v[3] * rotation.v[0];
            const auto wy = rotation.v[3] * rotation.v[1];
            const auto wz = rotation.v[3] * rotation.v[2];

            const auto xx = rotation.v[0] * rotation.v[0];
            const auto xy = rotation.v[0] * rotation.v[1];
            const auto xz = rotation.v[0] * rotation.v[2];

            const auto yy = rotation.v[1] * rotation.v[1];
            const auto yz = rotation.v[1] * rotation.v[2];

            const auto zz = rotation.v[2] * rotation.v[2];

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

        void setRotationX(const T angle) noexcept
        {
            static_assert(rows == 4 && cols == 4);

            setIdentity();

            const auto cosine = std::cos(angle);
            const auto sine = std::sin(angle);

            m[5] = cosine;
            m[9] = -sine;
            m[6] = sine;
            m[10] = cosine;
        }

        void setRotationY(const T angle) noexcept
        {
            static_assert(rows == 4 && cols == 4);

            setIdentity();

            const auto cosine = std::cos(angle);
            const auto sine = std::sin(angle);

            m[0] = cosine;
            m[8] = sine;
            m[2] = -sine;
            m[10] = cosine;
        }

        void setRotationZ(const T angle) noexcept
        {
            static_assert(rows == 4 && cols == 4);

            setIdentity();

            const auto cosine = std::cos(angle);
            const auto sine = std::sin(angle);

            m[0] = cosine;
            m[4] = -sine;
            m[1] = sine;
            m[5] = cosine;
        }

        void setTranslation(const Vector<T, rows - 1>& translation) noexcept
        {
            static_assert(cols == rows);

            setIdentity();

            for (std::size_t i = 0; i < rows - 1; ++i)
                m[(rows - 1) * cols + i] = translation[i];
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 3 && c == 3)>* = nullptr>
        void setTranslation(const T x, const T y) noexcept
        {
            setIdentity();

            m[6] = x;
            m[7] = y;
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 4 && c == 4)>* = nullptr>
        void setTranslation(const T x, const T y, const T z) noexcept
        {
            setIdentity();

            m[12] = x;
            m[13] = y;
            m[14] = z;
        }

        [[nodiscard]] auto getFrustumLeftPlane() const noexcept
        {
            static_assert(rows == 4 && cols == 4);
            return Plane<T>::makeFrustumPlane(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
        }

        [[nodiscard]] auto getFrustumRightPlane() const noexcept
        {
            static_assert(rows == 4 && cols == 4);
            return Plane<T>::makeFrustumPlane(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
        }

        [[nodiscard]] auto getFrustumBottomPlane() const noexcept
        {
            static_assert(rows == 4 && cols == 4);
            return Plane<T>::makeFrustumPlane(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13]);
        }

        [[nodiscard]] auto getFrustumTopPlane() const noexcept
        {
            static_assert(rows == 4 && cols == 4);
            return Plane<T>::makeFrustumPlane(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13]);
        }

        [[nodiscard]] auto getFrustumNearPlane() const noexcept
        {
            static_assert(rows == 4 && cols == 4);
            return Plane<T>::makeFrustumPlane(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14]);
        }

        [[nodiscard]] auto getFrustumFarPlane() const noexcept
        {
            static_assert(rows == 4 && cols == 4);
            return Plane<T>::makeFrustumPlane(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14]);
        }

        [[nodiscard]] auto getFrustum() const noexcept(false)
        {
            static_assert(rows == 4 && cols == 4);
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
            for (std::size_t i = 0; i < cols * rows; ++i)
                dst.m[i] = m[i] + scalar;
        }

        void add(const Matrix& matrix) noexcept
        {
            add(matrix, *this);
        }

        void add(const Matrix& matrix, Matrix& dst) const noexcept
        {
            for (std::size_t i = 0; i < cols * rows; ++i)
                dst.m[i] = m[i] + matrix.m[i];
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 1 && c == 1)>* = nullptr>
        [[nodiscard]] constexpr auto getDeterminant() const noexcept
        {
            return m[0];
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 2 && c == 2)>* = nullptr>
        [[nodiscard]] constexpr auto getDeterminant() const noexcept
        {
            return m[0] * m[3] - m[1] * m[2];
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 3 && c == 3)>* = nullptr>
        [[nodiscard]] constexpr auto getDeterminant() const noexcept
        {
            constexpr T a0 = m[0] * (m[4] * m[8] - m[5] * m[7]);
            constexpr T a1 = m[1] * (m[3] * m[8] - m[5] * m[6]);
            constexpr T a2 = m[2] * (m[3] * m[7] - m[4] * m[6]);
            return a0 - a1 + a2;
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 4 && c == 4)>* = nullptr>
        [[nodiscard]] constexpr auto getDeterminant() const noexcept
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

        [[nodiscard]] auto getUpVector() const noexcept
        {
            static_assert(rows == 4 && cols == 4);
            return Vector<T, 3>{m[4], m[5], m[6]};
        }

        [[nodiscard]] auto getDownVector() const noexcept
        {
            static_assert(rows == 4 && cols == 4);
            return Vector<T, 3>{-m[4], -m[5], -m[6]};
        }

        [[nodiscard]] auto getLeftVector() const noexcept
        {
            static_assert(rows == 4 && cols == 4);
            return Vector<T, 3>{-m[0], -m[1], -m[2]};
        }

        [[nodiscard]] auto getRightVector() const noexcept
        {
            static_assert(rows == 4 && cols == 4);
            return Vector<T, 3>{m[0], m[1], m[2]};
        }

        [[nodiscard]] auto getForwardVector() const noexcept
        {
            static_assert(rows == 4 && cols == 4);
            return Vector<T, 3>{-m[8], -m[9], -m[10]};
        }

        [[nodiscard]] auto getBackVector() const noexcept
        {
            static_assert(rows == 4 && cols == 4);
            return Vector<T, 3>{m[8], m[9], m[10]};
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 1 && c == 1)>* = nullptr>
        void invert() noexcept
        {
            const auto determinant = m[0];

            // Close to zero, can't invert
            if (std::fabs(determinant) <= std::numeric_limits<T>::epsilon()) return;

            m[0] = T(1) / m[0];
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 2 && c == 2)>* = nullptr>
        void invert() noexcept
        {
            const auto determinant = m[0] * m[3] - m[1] * m[2];

            // Close to zero, can't invert
            if (std::fabs(determinant) <= std::numeric_limits<T>::epsilon()) return;

            const Matrix adjugate{
                m[3], -m[1],
                -m[2], m[0]
            };

            adjugate.multiply(T(1) / determinant, *this);
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 3 && c == 3)>* = nullptr>
        void invert() noexcept
        {
            const auto a0 = m[0] * (m[4] * m[8] - m[5] * m[7]);
            const auto a1 = m[1] * (m[3] * m[8] - m[5] * m[6]);
            const auto a2 = m[2] * (m[3] * m[7] - m[4] * m[6]);

            const auto determinant = a0 - a1 + a2;

            // Close to zero, can't invert
            if (std::fabs(determinant) <= std::numeric_limits<T>::epsilon()) return;

            const Matrix adjugate{
                m[4] * m[8] - m[5] * m[7],
                -(m[3] * m[8] - m[5] * m[6]),
                m[3] * m[7] - m[4] * m[6],
                -(m[1] * m[8] - m[2] * m[7]),
                m[0] * m[8] - m[2] * m[6],
                -(m[0] * m[7] - m[1] * m[6]),
                m[1] * m[5] - m[2] * m[4],
                -(m[0] * m[5] - m[2] * m[3]),
                m[0] * m[4] - m[1] * m[3]
            };

            adjugate.multiply(T(1) / determinant, *this);
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 4 && c == 4)>* = nullptr>
        void invert() noexcept
        {
            const auto a0 = m[0] * m[5] - m[1] * m[4];
            const auto a1 = m[0] * m[6] - m[2] * m[4];
            const auto a2 = m[0] * m[7] - m[3] * m[4];
            const auto a3 = m[1] * m[6] - m[2] * m[5];
            const auto a4 = m[1] * m[7] - m[3] * m[5];
            const auto a5 = m[2] * m[7] - m[3] * m[6];
            const auto b0 = m[8] * m[13] - m[9] * m[12];
            const auto b1 = m[8] * m[14] - m[10] * m[12];
            const auto b2 = m[8] * m[15] - m[11] * m[12];
            const auto b3 = m[9] * m[14] - m[10] * m[13];
            const auto b4 = m[9] * m[15] - m[11] * m[13];
            const auto b5 = m[10] * m[15] - m[11] * m[14];

            const auto determinant = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

            // Close to zero, can't invert
            if (std::fabs(determinant) <= std::numeric_limits<T>::epsilon()) return;

            const Matrix adjugate{
                m[5] * b5 - m[6] * b4 + m[7] * b3,
                -m[1] * b5 + m[2] * b4 - m[3] * b3,
                m[13] * a5 - m[14] * a4 + m[15] * a3,
                -m[9] * a5 + m[10] * a4 - m[11] * a3,

                -m[4] * b5 + m[6] * b2 - m[7] * b1,
                m[0] * b5 - m[2] * b2 + m[3] * b1,
                -m[12] * a5 + m[14] * a2 - m[15] * a1,
                m[8] * a5 - m[10] * a2 + m[11] * a1,

                m[4] * b4 - m[5] * b2 + m[7] * b0,
                -m[0] * b4 + m[1] * b2 - m[3] * b0,
                m[12] * a4 - m[13] * a2 + m[15] * a0,
                -m[8] * a4 + m[9] * a2 - m[11] * a0,

                -m[4] * b3 + m[5] * b1 - m[6] * b0,
                m[0] * b3 - m[1] * b1 + m[2] * b0,
                -m[12] * a3 + m[13] * a1 - m[14] * a0,
                m[8] * a3 - m[9] * a1 + m[10] * a0
            };

            adjugate.multiply(T(1) / determinant, *this);
        }

        [[nodiscard]] constexpr auto isIdentity() const noexcept
        {
            if constexpr (cols != rows) return false;

            for (std::size_t i = 0; i < rows; ++i)
                for (std::size_t j = 0; j < cols; ++j)
                    if (m[i * cols + j] != (i == j ? T(1) : T(0)))
                        return false;
            return true;
        }

        void multiply(const T scalar) noexcept
        {
            multiply(scalar, *this);
        }

        void multiply(const T scalar, Matrix& dst) const noexcept
        {
            for (std::size_t i = 0; i < cols * rows; ++i)
                dst.m[i] = m[i] * scalar;
        }

        void multiply(const Matrix& matrix) noexcept
        {
            multiply(matrix, *this);
        }

        void multiply(const Matrix& matrix, Matrix& dst) const noexcept
        {
            std::array<T, cols * rows> result{};

            for (std::size_t i = 0; i < rows; ++i)
                for (std::size_t j = 0; j < cols; ++j)
                    for (std::size_t k = 0; k < rows; ++k)
                        result[i * cols + j] += m[k * cols + j] * matrix.m[i * cols + k];

            std::copy(result.begin(), result.end(), dst.m.begin());
        }

        void negate() noexcept
        {
            for (auto& c : m)
                c = -c;
        }

        void setIdentity() noexcept
        {
            static_assert(cols == rows);

            for (std::size_t i = 0; i < rows; ++i)
                for (std::size_t j = 0; j < cols; ++j)
                    m[i * cols + j] = i == j ? T(1) : T(0);
        }

        void setZero() noexcept
        {
            for (auto& c : m)
                c = T(0);
        }

        void subtract(const T scalar) noexcept
        {
            subtract(scalar, *this);
        }

        void subtract(const T scalar, Matrix& dst) const noexcept
        {
            for (std::size_t i = 0; i < cols * rows; ++i)
                dst.m[i] = m[i] - scalar;
        }

        void subtract(const Matrix& matrix) noexcept
        {
            subtract(matrix, *this);
        }

        void subtract(const Matrix& matrix, Matrix& dst) const noexcept
        {
            for (std::size_t i = 0; i < cols * rows; ++i)
                dst.m[i] = m[i] - matrix.m[i];
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 4 && c == 4)>* = nullptr>
        void transformPoint(Vector<T, 3>& point) const noexcept
        {
            transformVector(point.v[0], point.v[1], point.v[2], T(1), point);
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 4 && c == 4)>* = nullptr>
        void transformPoint(const Vector<T, 3>& point, Vector<T, 3>& dst) const noexcept
        {
            transformVector(point.v[0], point.v[1], point.v[2], T(1), dst);
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 4 && c == 4)>* = nullptr>
        void transformVector(Vector<T, 3>& v) const noexcept
        {
            Vector<T, 4> t;
            transformVector(Vector<T, 4>{v.v[0], v.v[1], v.v[2], T(0)}, t);
            v = Vector<T, 3>{t.v[0], t.v[1], t.v[2]};
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 4 && c == 4)>* = nullptr>
        void transformVector(const Vector<T, 3>& v, Vector<T, 3>& dst) const noexcept
        {
            transformVector(v.v[0], v.v[1], v.v[2], T(0), dst);
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 4 && c == 4)>* = nullptr>
        void transformVector(const T x, const T y, const T z, const T w,
                             Vector<T, 3>& dst) const noexcept
        {
            Vector<T, 4> t;
            transformVector(Vector<T, 4>{x, y, z, w}, t);
            dst = Vector<T, 3>{t.v[0], t.v[1], t.v[2]};
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 4 && c == 4)>* = nullptr>
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

        void transpose() noexcept
        {
            static_assert(cols == rows);
            transpose(*this);
        }

        void transpose(Matrix& dst) const noexcept
        {
            const std::array<T, cols * rows> t;

            for (std::size_t i = 0; i < cols; ++i)
                for (std::size_t j = 0; j < cols; ++j)
                    t[j * cols + i] = t[i * rows + j];

            std::copy(t.begin(), t.end(), dst.m.begin());
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 3 && c == 3)>* = nullptr>
        [[nodiscard]] constexpr auto getTranslation() const noexcept
        {
            return Vector<T, 2>{m[6], m[7]};
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 4 && c == 4)>* = nullptr>
        [[nodiscard]] constexpr auto getTranslation() const noexcept
        {
            return Vector<T, 3>{m[12], m[13], m[14]};
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 3 && c == 3)>* = nullptr>
        [[nodiscard]] auto getScale() const noexcept
        {
            Vector<T, 2> scale;
            scale.v[0] = Vector<T, 2>{m[0], m[1]}.length();
            scale.v[1] = Vector<T, 2>{m[3], m[4]}.length();

            return scale;
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 4 && c == 4)>* = nullptr>
        [[nodiscard]] auto getScale() const noexcept
        {
            Vector<T, 3> scale;
            scale.v[0] = Vector<T, 3>{m[0], m[1], m[2]}.length();
            scale.v[1] = Vector<T, 3>{m[4], m[5], m[6]}.length();
            scale.v[2] = Vector<T, 3>{m[8], m[9], m[10]}.length();

            return scale;
        }

        template <auto r = rows, auto c = cols, std::enable_if_t<(r == 4 && c == 4)>* = nullptr>
        [[nodiscard]] auto getRotation() const noexcept
        {
            const auto scale = getScale();

            const auto m11 = m[0] / scale.v[0];
            const auto m21 = m[1] / scale.v[0];
            const auto m31 = m[2] / scale.v[0];

            const auto m12 = m[4] / scale.v[1];
            const auto m22 = m[5] / scale.v[1];
            const auto m32 = m[6] / scale.v[1];

            const auto m13 = m[8] / scale.v[2];
            const auto m23 = m[9] / scale.v[2];
            const auto m33 = m[10] / scale.v[2];

            Quaternion<T> result{
                std::sqrt(std::max(T(0), T(1) + m11 - m22 - m33)) / T(2),
                std::sqrt(std::max(T(0), T(1) - m11 + m22 - m33)) / T(2),
                std::sqrt(std::max(T(0), T(1) - m11 - m22 + m33)) / T(2),
                std::sqrt(std::max(T(0), T(1) + m11 + m22 + m33)) / T(2)
            };

            // The problem with using copysign: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/paul.htm
            result.v[0] = std::copysign(result.v[0], m32 - m23);
            result.v[1] = std::copysign(result.v[1], m13 - m31);
            result.v[2] = std::copysign(result.v[2], m21 - m12);

            result.normalize();

            return result;
        }

        [[nodiscard]] auto operator+() const noexcept
        {
            return *this;
        }

        [[nodiscard]] auto operator-() const noexcept
        {
            Matrix result(*this);
            result.negate();
            return result;
        }

        [[nodiscard]] auto operator+(const Matrix& matrix) const noexcept
        {
            Matrix result(*this);
            result.add(matrix);
            return result;
        }

        auto& operator+=(const Matrix& matrix) noexcept
        {
            add(matrix);
            return *this;
        }

        [[nodiscard]] auto operator-(const Matrix& matrix) const noexcept
        {
            Matrix result(*this);
            result.subtract(matrix);
            return result;
        }

        auto& operator-=(const Matrix& matrix) noexcept
        {
            subtract(matrix);
            return *this;
        }

        [[nodiscard]] auto operator*(const Matrix& matrix) const noexcept
        {
            Matrix result(*this);
            result.multiply(matrix);
            return result;
        }

        auto& operator*=(const Matrix& matrix) noexcept
        {
            multiply(matrix);
            return *this;
        }

        [[nodiscard]] auto operator*(const T scalar) const noexcept
        {
            Matrix result(*this);
            result.multiply(scalar);
            return result;
        }

        auto& operator*=(const T scalar) noexcept
        {
            multiply(scalar);
            return *this;
        }

        [[nodiscard]] constexpr auto operator==(const Matrix& mat) const noexcept
        {
            return std::equal(std::begin(m), std::end(m), std::begin(mat.m));
        }

        [[nodiscard]] constexpr auto operator!=(const Matrix& mat) const noexcept
        {
            return !std::equal(std::begin(m), std::end(m), std::begin(mat.m));
        }

        [[nodiscard]] auto operator*(const Vector<T, 3>& v) const noexcept
        {
            static_assert(rows == 4 && cols == 4);

            Vector<T, 3> x;
            transformVector(v, x);
            return x;
        }

        [[nodiscard]] auto operator*(const Vector<T, 4>& v) const noexcept
        {
            static_assert(rows == 4 && cols == 4);

            Vector<T, 4> x;
            transformVector(v, x);
            return x;
        }

    private:
        template <std::size_t ...I>
        static constexpr auto generateIdentity(const std::index_sequence<I...>) noexcept
        {
            return Matrix{(I % cols == I / rows) ? T(1) : T(0)...};
        }
    };

    template <typename T>
    [[nodiscard]] auto& operator*=(Vector<T, 3>& v, const Matrix<T, 4, 4>& m) noexcept
    {
        m.transformVector(v);
        return v;
    }

    template <typename T>
    [[nodiscard]] auto& operator*=(Vector<T, 4>& v, const Matrix<T, 4, 4>& m) noexcept
    {
        m.transformVector(v);
        return v;
    }

    template <typename T, std::size_t cols, std::size_t rows>
    [[nodiscard]] auto operator*(const T scalar, const Matrix<T, cols, rows>& m) noexcept
    {
        return m * scalar;
    }
}

#endif // OUZEL_MATH_MATRIX_HPP
