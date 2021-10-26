// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_CONVEXVOLUME_HPP
#define OUZEL_MATH_CONVEXVOLUME_HPP

#include <type_traits>
#include <vector>
#include "Box.hpp"
#include "Plane.hpp"
#include "Vector.hpp"

namespace ouzel::math
{
    template <typename T> class ConvexVolume final
    {
    public:
        std::vector<Plane<T>> planes;

        ConvexVolume() noexcept {}

        explicit ConvexVolume(const std::vector<Plane<T>>& initPlanes) noexcept:
            planes{initPlanes}
        {
        }
    };

    template <typename T, std::size_t dims, std::enable_if_t<(dims >= 3)>* = nullptr>
    [[nodiscard]] auto isPointInside(const ConvexVolume<T> volume,
                                     const math::Vector<T, dims>& position) noexcept
    {
        for (const auto& plane : volume.planes)
            if (dot(plane, position) < T(0))
                return false;

        return true;
    }

    template <typename T, std::size_t dims, std::enable_if_t<(dims >= 3)>* = nullptr>
    [[nodiscard]] auto isSphereInside(const ConvexVolume<T> volume,
                                      const math::Vector<T, dims>& position,
                                      const T radius) noexcept
    {
        for (const auto& plane : volume.planes)
            if (dot(plane, position) < -radius)
                return false;

        return true;
    }

    template <typename T>
    [[nodiscard]] auto isBoxInside(const ConvexVolume<T> volume,
                                   const math::Box<T, 3>& box) noexcept
    {
        const math::Vector<T, 3> leftBottomBack{box.min.v[0], box.min.v[1], box.min.v[2]};
        const math::Vector<T, 3> leftBottomFront{box.min.v[0], box.min.v[1], box.max.v[2]};
        const math::Vector<T, 3> leftTopBack{box.min.v[0], box.max.v[1], box.min.v[2]};
        const math::Vector<T, 3> leftTopFront{box.min.v[0], box.max.v[1], box.max.v[2]};
        const math::Vector<T, 3> rightBottomBack{box.max.v[0], box.min.v[1], box.min.v[2]};
        const math::Vector<T, 3> rightBottomFront{box.max.v[0], box.min.v[1], box.max.v[2]};
        const math::Vector<T, 3> rightTopBack{box.max.v[0], box.max.v[1], box.min.v[2]};
        const math::Vector<T, 3> rightTopFront{box.max.v[0], box.max.v[1], box.max.v[2]};

        for (const auto& plane : volume.planes)
            if (dot(plane, leftBottomBack) < T(0) &&
                dot(plane, leftBottomFront) < T(0) &&
                dot(plane, leftTopBack) < T(0) &&
                dot(plane, leftTopFront) < T(0) &&
                dot(plane, rightBottomBack) < T(0) &&
                dot(plane, rightBottomFront) < T(0) &&
                dot(plane, rightTopBack) < T(0) &&
                dot(plane, rightTopFront) < T(0))
                return false;

        return true;
    }
}

#endif // OUZEL_MATH_CONVEXVOLUME_HPP
