// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Component.h"
#include "Node.h"
#include "utils/Utils.h"
#include "math/MathUtils.h"

namespace ouzel
{
    namespace scene
    {
        Component::~Component()
        {
            if (node) node->removeComponent(this);
        }

        void Component::draw(const Matrix4&,
                             const graphics::Color&,
                             scene::Camera*)
        {
        }

        void Component::drawWireframe(const Matrix4&,
                                      const graphics::Color&,
                                      scene::Camera*)
        {
        }

        bool Component::pointOn(const Vector2& position) const
        {
            return boundingBox.containsPoint(position);
        }

        inline void gatherPolygonProjectionExtents(const std::vector<Vector2>& vertList,
                                                   const ouzel::Vector2& v,
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

        inline bool findSeparatingAxis(const std::vector<Vector2>& aVertList,
                                       const std::vector<Vector2>& bVertList)
        {
            ouzel::Vector2 v;

            size_t aVertListSize = aVertList.size();
            size_t prev = aVertListSize - 1;
            for (size_t cur = 0; cur < aVertListSize; ++cur)
            {
                ouzel::Vector2 edge = aVertList[cur] - aVertList[prev];

                v.x = edge.y;
                v.y = -edge.x;

                float aMin, aMax, bMin, bMax;
                gatherPolygonProjectionExtents(aVertList, v, aMin, aMax);
                gatherPolygonProjectionExtents(bVertList, v, bMin, bMax);

                if (aMax < bMin) return true;
                if (bMax < aMin) return true;

                prev = cur;
            }

            return false;
        }

        bool Component::shapeOverlaps(const std::vector<Vector2>& edges) const
        {
            std::vector<Vector2> boundingBoxEdges = {
                Vector2(boundingBox.min),
                Vector2(boundingBox.max.x, boundingBox.min.y),
                Vector2(boundingBox.max),
                Vector2(boundingBox.min.x, boundingBox.max.y)
            };

            if (findSeparatingAxis(boundingBoxEdges, edges))
                return false;

            if (findSeparatingAxis(edges, boundingBoxEdges))
                return false;

            return true;
        }
    } // namespace scene
} // namespace ouzel
