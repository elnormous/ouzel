// Ouzel by Elviss Strazdins

#include "Component.hpp"
#include "Actor.hpp"

namespace ouzel::scene
{
    Component::~Component()
    {
        if (actor) actor->removeComponent(*this);
    }

    void Component::draw(const math::Matrix<float, 4>&,
                         float,
                         const math::Matrix<float, 4>&,
                         bool)
    {
    }

    bool Component::pointOn(const math::Vector<float, 2>& position) const
    {
        return containsPoint(boundingBox, math::Vector<float, 3>{position});
    }

    namespace
    {
        template <class Iterator>
        auto gatherPolygonProjectionExtents(const Iterator begin, const Iterator end,
                                            const math::Vector<float, 2>& v) noexcept
        {
            std::pair<float, float> result{};

            if (auto i = begin; i != end)
            {
                result.first = result.second = dot(v, *i);
                ++i;

                for (; i != end; ++i)
                {
                    const float d = dot(v, *i);
                    if (d < result.first) result.first = d; // min
                    else if (d > result.second) result.second = d; // max
                }
            }

            return result;
        }

        template <class IteratorA, class IteratorB>
        bool findSeparatingAxis(const IteratorA aBegin, const IteratorA aEnd,
                                const IteratorB bBegin, const IteratorB bEnd) noexcept
        {
            auto prev = aEnd - 1;
            for (auto cur = aBegin; cur != aEnd; ++cur)
            {
                const math::Vector<float, 2> edge = *cur - *prev;
                const math::Vector<float, 2> v{
                    edge.v[1],
                    -edge.v[0]
                };

                const auto [aMin, aMax] = gatherPolygonProjectionExtents(aBegin, aEnd, v);
                const auto [bMin, bMax] = gatherPolygonProjectionExtents(bBegin, bEnd, v);

                if (aMax < bMin) return true;
                if (bMax < aMin) return true;

                prev = cur;
            }

            return false;
        }
    }

    bool Component::shapeOverlaps(const std::vector<math::Vector<float, 2>>& edges) const
    {
        const auto boundingBoxEdges = {
            math::Vector<float, 2>{boundingBox.min},
            math::Vector<float, 2>{boundingBox.max.v[0], boundingBox.min.v[1]},
            math::Vector<float, 2>{boundingBox.max},
            math::Vector<float, 2>{boundingBox.min.v[0], boundingBox.max.v[1]}
        };

        if (findSeparatingAxis(boundingBoxEdges.begin(), boundingBoxEdges.end(),
                               edges.begin(), edges.end()))
            return false;

        if (findSeparatingAxis(edges.begin(), edges.end(),
                               boundingBoxEdges.begin(), boundingBoxEdges.end()))
            return false;

        return true;
    }

    void Component::removeFromActor()
    {
        if (actor) actor->removeComponent(*this);
    }

    void Component::setActor(Actor* newActor)
    {
        actor = newActor;
    }

    void Component::setLayer(Layer* newLayer)
    {
        layer = newLayer;
    }

    void Component::updateTransform()
    {
    }
}
