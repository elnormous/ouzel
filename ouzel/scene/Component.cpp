// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Component.hpp"
#include "Actor.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace scene
    {
        Component::Component(uint32_t initClass):
            cls(initClass)
        {
        }

        Component::~Component()
        {
            if (actor) actor->removeComponent(this);
        }

        void Component::draw(const Matrix4F&,
                             float,
                             const Matrix4F&,
                             bool)
        {
        }

        bool Component::pointOn(const Vector2F& position) const
        {
            return boundingBox.containsPoint(Vector3F(position));
        }

        inline void gatherPolygonProjectionExtents(const std::vector<Vector2F>& vertList,
                                                   const Vector2F& v,
                                                   float& outMin, float& outMax)
        {
            outMin = outMax = v.dot(vertList[0]);

            size_t vertListSize = vertList.size();
            for (size_t i = 1 ; i < vertListSize; ++i)
            {
                float d = v.dot(vertList[i]);
                if (d < outMin) outMin = d;
                else if (d > outMax) outMax = d;
            }
        }

        inline bool findSeparatingAxis(const std::vector<Vector2F>& aVertList,
                                       const std::vector<Vector2F>& bVertList)
        {
            Vector2F v;

            size_t aVertListSize = aVertList.size();
            size_t prev = aVertListSize - 1;
            for (size_t cur = 0; cur < aVertListSize; ++cur)
            {
                Vector2F edge = aVertList[cur] - aVertList[prev];

                v.v[0] = edge.v[1];
                v.v[1] = -edge.v[0];

                float aMin;
                float aMax;
                float bMin;
                float bMax;
                gatherPolygonProjectionExtents(aVertList, v, aMin, aMax);
                gatherPolygonProjectionExtents(bVertList, v, bMin, bMax);

                if (aMax < bMin) return true;
                if (bMax < aMin) return true;

                prev = cur;
            }

            return false;
        }

        bool Component::shapeOverlaps(const std::vector<Vector2F>& edges) const
        {
            std::vector<Vector2F> boundingBoxEdges = {
                Vector2F(boundingBox.min),
                Vector2F(boundingBox.max.v[0], boundingBox.min.v[1]),
                Vector2F(boundingBox.max),
                Vector2F(boundingBox.min.v[0], boundingBox.max.v[1])
            };

            if (findSeparatingAxis(boundingBoxEdges, edges))
                return false;

            if (findSeparatingAxis(edges, boundingBoxEdges))
                return false;

            return true;
        }

        void Component::removeFromActor()
        {
            if (actor) actor->removeComponent(this);
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
    } // namespace scene
} // namespace ouzel
