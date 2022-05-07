// Ouzel by Elviss Strazdins

#ifndef OUZEL_EASING_EASING_HPP
#define OUZEL_EASING_EASING_HPP

#include <cmath>

namespace ouzel::easing
{
    enum class Mode
    {
        easeIn,
        easeOut,
        easeInOut
    };

    enum class Func
    {
        sine,
        quad,
        cubic,
        quart,
        quint,
        expo,
        circ,
        back,
        elastic,
        bounce
    };

    template <typename T> constexpr auto tau = T(6.28318530717958647692);

    template <typename T>
    auto sineIn(const T t) noexcept
    {
        return T(1) - std::cos(t * tau<T> / T(4));
    }

    template <typename T>
    auto sineOut(const T t) noexcept
    {
        return std::sin(t * tau<T> / T(4));
    }

    template <typename T>
    auto sineInOut(const T t) noexcept
    {
        return -T(0.5) * (std::cos(t * tau<T> / T(2)) - T(1));
    }

    template <typename T>
    constexpr auto quadIn(const T t) noexcept
    {
        return t * t;
    }

    template <typename T>
    constexpr auto quadOut(const T t) noexcept
    {
        return t * (T(2) - t);
    }

    template <typename T>
    constexpr auto quadInOut(const T t) noexcept
    {
        return (t < T(0.5)) ?
            T(2) * t * t :
            -T(1) + (T(4) - T(2) * t) * t;
    }

    template <typename T>
    constexpr auto cubicIn(const T t) noexcept
    {
        return t * t * t;
    }

    template <typename T>
    constexpr auto cubicOut(const T t) noexcept
    {
        return (t - T(1)) * (t - T(1)) * (t - T(1)) + T(1);
    }

    template <typename T>
    constexpr auto cubicInOut(const T t) noexcept
    {
        return (t < T(0.5)) ?
            T(4) * t * t * t :
            (t - T(1)) * (T(2) * t - T(2)) * (T(2) * t - T(2)) + T(1);
    }

    template <typename T>
    constexpr auto quartIn(const T t) noexcept
    {
        return t * t * t * t;
    }

    template <typename T>
    constexpr auto quartOut(const T t) noexcept
    {
        return T(1) - (t - T(1)) * (t - T(1)) * (t - T(1));
    }

    template <typename T>
    constexpr auto quartInOut(const T t) noexcept
    {
        return (t < T(0.5)) ?
            T(8) * t * t * t * t :
            T(1) - T(8) * (t - T(1)) * (t - T(1)) * (t - T(1)) * (t - T(1));
    }

    template <typename T>
    constexpr auto quintIn(const T t) noexcept
    {
        return t * t * t * t * t;
    }

    template <typename T>
    constexpr auto quintOut(const T t) noexcept
    {
        return T(1) + (t - T(1)) * (t - T(1)) * (t - T(1)) * (t - T(1)) * (t - T(1));
    }

    template <typename T>
    constexpr auto quintInOut(const T t) noexcept
    {
        return (t < T(0.5)) ?
            T(16) * t * t * t * t * t :
            T(1) + T(16) * (t - T(1)) * (t - T(1)) * (t - T(1)) * (t - T(1)) * (t - T(1));
    }

    template <typename T>
    auto expoIn(const T t) noexcept
    {
        return std::pow(T(2), T(10) * (t - T(1)));
    }

    template <typename T>
    auto expoOut(const T t) noexcept
    {
        return T(1) - std::pow(T(2), -T(10) * t);
    }

    template <typename T>
    auto expoInOut(const T t) noexcept
    {
        return (t < T(0.5)) ?
            T(0.5) * std::pow(T(2), T(10) * (T(2) * t - T(1))) :
            T(0.5) * (std::pow(T(2), -T(10) * (t * T(2) - T(1))) - T(2));
    }

    template <typename T>
    auto circIn(const T t) noexcept
    {
        return T(1) - std::sqrt(T(1) - t * t);
    }

    template <typename T>
    auto circOut(const T t) noexcept
    {
        return std::sqrt(T(1) - (t - T(1)) * (t - T(1)));
    }

    template <typename T>
    auto circInOut(const T t) noexcept
    {
        return (t < T(0.5)) ?
            T(0.5) * (-std::sqrt(T(1) - (t * T(2)) * (t * T(2))) + T(1)) :
            T(0.5) * (std::sqrt(T(1) - (t * T(2) - T(2)) * (t * T(2) - T(2))) + T(1));
    }

    template <typename T>
    constexpr auto backIn(const T t) noexcept
    {
        constexpr T s = T(1.70158);
        return t * t * ((s + T(1)) * t - s);
    }

    template <typename T>
    constexpr auto backOut(const T t) noexcept
    {
        constexpr T s = T(1.70158);
        return (t - T(1)) * (t - T(1)) * ((s + T(1)) * (t - T(1)) + s) + T(1);
    }

    template <typename T>
    constexpr auto backInOut(const T t) noexcept
    {
        constexpr T s = T(1.70158) * T(1.525);
        return (t < T(0.5)) ?
            T(0.5) * ((t * T(2)) * (t * T(2)) * ((s + T(1)) * (t * T(2)) - s)):
            T(0.5) * ((t * T(2) - T(2)) * (t * T(2) - T(2)) * ((s + T(1)) * (t * T(2) - T(2)) + s) + T(2));
    }

    template <typename T>
    auto elasticIn(const T t) noexcept
    {
        if (t == T(0)) return T(0);
        if (t == T(1)) return T(1);

        static constexpr T p = T(0.3);

        return -std::pow(T(2), T(10) * (t - T(1))) * std::sin(((t - T(1)) - p / T(4)) * tau<T> / p);
    }

    template <typename T>
    auto elasticOut(const T t) noexcept
    {
        if (t == T(0)) return T(0);
        if (t == T(1)) return T(1);

        static constexpr T p = T(0.3);

        return std::pow(T(2), -T(10) * t) * std::sin((t - p / T(4)) * tau<T> / p) + T(1);
    }

    template <typename T>
    auto elasticInOut(const T t) noexcept
    {
        if (t == T(0)) return T(0);
        if (t == T(1)) return T(1);

        static constexpr T p = T(0.3) * T(1.5);

        return (t < T(0.5)) ?
            -T(0.5) * std::pow(T(2), T(10) * (t * T(2) - T(1))) * std::sin(((t * T(2) - T(1)) - p / T(4)) * tau<T> / p) :
            T(0.5) * std::pow(T(2), -T(10) * (t * T(2) - T(1))) * std::sin(((t * T(2) - T(1)) - p / T(4)) * tau<T> / p) + T(1);
    }

    template <typename T>
    constexpr auto bounceOut(const T t) noexcept
    {
        return (t < T(1) / T(2.75)) ? T(7.5625) * t * t :
            (t < T(2) / T(2.75)) ? T(7.5625) * (t - T(1.5) / T(2.75)) * (t - T(1.5) / T(2.75)) + T(0.75) :
            (t < T(2.5) / T(2.75)) ? T(7.5625) * (t - T(2.25) / T(2.75)) * (t - T(2.25) / T(2.75)) + T(0.9375) :
            T(7.5625) * (t - T(2.625) / T(2.75)) * (t - T(2.625) / T(2.75)) + T(0.984375);
    }

    template <typename T>
    constexpr auto bounceIn(const T t) noexcept
    {
        return T(1) - bounceOut(T(1) - t);
    }

    template <typename T>
    constexpr auto bounceInOut(const T t) noexcept
    {
        return (t < T(0.5)) ?
            bounceOut(t * T(2)) * T(0.5) :
            bounceOut(t * T(2) - T(1)) * T(0.5) + T(0.5);
    }

    template <typename T>
    auto ease(const Func func, const Mode mode, const T t) noexcept
    {
        switch (mode)
        {
            case Mode::easeIn:
            {
                switch (func)
                {
                    case Func::sine: return sineIn(t);
                    case Func::quad: return quadIn(t);
                    case Func::cubic: return cubicIn(t);
                    case Func::quart: return quartIn(t);
                    case Func::quint: return quintIn(t);
                    case Func::expo: return expoIn(t);
                    case Func::circ: return circIn(t);
                    case Func::back: return backIn(t);
                    case Func::elastic: return elasticIn(t);
                    case Func::bounce: return bounceIn(t);
                }
                break;
            }

            case Mode::easeOut:
            {
                switch (func)
                {
                    case Func::sine: return sineOut(t);
                    case Func::quad: return quadOut(t);
                    case Func::cubic: return cubicOut(t);
                    case Func::quart: return quartOut(t);
                    case Func::quint: return quintOut(t);
                    case Func::expo: return expoOut(t);
                    case Func::circ: return circOut(t);
                    case Func::back: return backOut(t);
                    case Func::elastic: return elasticOut(t);
                    case Func::bounce: return bounceOut(t);
                }
                break;
            }

            case Mode::easeInOut:
            {
                switch (func)
                {
                    case Func::sine: return sineInOut(t);
                    case Func::quad: return quadInOut(t);
                    case Func::cubic: return cubicInOut(t);
                    case Func::quart: return quartInOut(t);
                    case Func::quint: return quintInOut(t);
                    case Func::expo: return expoInOut(t);
                    case Func::circ: return circInOut(t);
                    case Func::back: return backInOut(t);
                    case Func::elastic: return elasticInOut(t);
                    case Func::bounce: return bounceInOut(t);
                }
                break;
            }
        }

        return T(0);
    }
}

#endif // OUZEL_EASING_EASING_HPP
