// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_SIZE_HPP
#define OUZEL_MATH_SIZE_HPP

#include <cstddef>
#include "math/Vector.hpp"

namespace ouzel
{
    template<size_t N, class T> class Size final
    {
    public:
        T v[N]{0};

        Size()
        {
        }

        template<typename ...A>
        Size(A... args):
            v{static_cast<T>(args)...}
        {
        }

        template<size_t N2, bool E = (N != N2), typename std::enable_if<E>::type* = nullptr>
        explicit Size(const Size<N2, T>& size)
        {
            for (size_t i = 0; i < N && i < N2; ++i)
                v[i] = size.v[i];
        }

        explicit Size(const Vector<N, T>& vec)
        {
            for (size_t i = 0; i < N; ++i)
                v[i] = vec.v[i];
        }

        inline T& operator[](size_t index) { return v[index]; }
        inline T operator[](size_t index) const { return v[index]; }

        inline T& width() { return v[0]; }
        inline T width() const { return v[0]; }

        inline T& height() { return v[1]; }
        inline T height() const { return v[1]; }

        inline T& depth() { return v[2]; }
        inline T depth() const { return v[2]; }

        void scale(const Vector<N, T>& scale)
        {
            for (size_t i = 0; i < N; ++i)
                v[i] *= scale.v[i];
        }

        inline const Size operator+(const Size& size) const
        {
            Size result = *this;
            for (size_t i = 0; i < N; ++i)
                result.v[i] += size.v[i];
            return result;
        }

        inline Size& operator+=(const Size& size)
        {
            for (size_t i = 0; i < N; ++i)
                v[i] += size.v[i];
            return *this;
        }

        inline const Size operator-(const Size& size) const
        {
            Size result = *this;
            for (size_t i = 0; i < N; ++i)
                result.v[i] -= size.v[i];
            return result;
        }

        inline Size& operator-=(const Size& size)
        {
            for (size_t i = 0; i < N; ++i)
                v[i] -= size.v[i];
            return *this;
        }

        inline const Size operator-() const
        {
            Size result = *this;
            for (T& c : result.v)
                c = -c;
            return result;
        }

        inline const Size operator*(T scalar) const
        {
            Size result(*this);
            for (T& c : result.v)
                c *= scalar;
            return result;
        }

        inline Size& operator*=(T scalar)
        {
            for (T& c : v)
                c *= scalar;
            return *this;
        }

        inline const Size operator/(T scalar) const
        {
            Size result(*this);
            for (T& c : result.v)
                c /= scalar;
            return result;
        }

        inline Size& operator/=(T scalar)
        {
            for (T& c : v)
                c /= scalar;
            return *this;
        }

        inline bool operator==(const Size& size) const
        {
            for (size_t i = 0; i < N; ++i)
                if (v[i] != size.v[i]) return false;
            return true;
        }

        inline bool operator!=(const Size& size) const
        {
            for (size_t i = 0; i < N; ++i)
                if (v[i] != size.v[i]) return true;
            return false;
        }

        bool isZero() const
        {
            for (const T& c : v)
                if (c != 0) return false;
            return true;
        }

        inline T volume() const
        {
            T result = 0;
            for (const T& c : v)
                result *= c;

            return result;
        }
    };

    template<size_t N, class T>
    inline const Size<N, T> operator*(const Size<N, T>& size, const Vector<N, T>& v)
    {
        Size<N, T> result = size;
        for (size_t i = 0; i < N; ++i)
            result.v[i] *= v.v[i];
        return result;
    }

    template<size_t N, class T>
    inline const Size<N, T> operator/(const Size<N, T>& size, const Vector<N, T>& v)
    {
        Size<N, T> result = size;
        for (size_t i = 0; i < N; ++i)
            result.v[i] /= v.v[i];
        return result;
    }

    using Size2U = Size<2, uint32_t>;
    using Size3U = Size<3, uint32_t>;
    using Size2F = Size<2, float>;
    using Size3F = Size<3, float>;
}

#endif // OUZEL_MATH_SIZE_HPP
