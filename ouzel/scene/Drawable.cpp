// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Drawable.h"
#include "utils/Utils.h"
#include "math/MathUtils.h"

namespace ouzel
{
    namespace scene
    {
        Drawable::~Drawable()
        {
        }

        void Drawable::draw(const Matrix4&,
                            const Matrix4&,
                            const graphics::Color&,
                            const graphics::RenderTargetPtr&,
                            const NodePtr&)
        {
        }

        bool Drawable::pointOn(const Vector2& position) const
        {
            return boundingBox.containsPoint(position);
        }

        bool Drawable::shapeOverlaps(const std::vector<Vector2>& edges) const
        {
            uint8_t inCorners = 0;

            for (const Vector2& corner : edges)
            {
                if (corner.x >= boundingBox.min.x &&
                    corner.x <= boundingBox.max.x &&
                    corner.y >= boundingBox.min.y &&
                    corner.y <= boundingBox.max.y)
                {
                    return true;
                }

                if (corner.x < boundingBox.min.x && corner.y < boundingBox.min.y) inCorners |= 0x01;
                if (corner.x > boundingBox.max.x && corner.y < boundingBox.min.y) inCorners |= 0x02;
                if (corner.x > boundingBox.max.x && corner.y > boundingBox.max.y) inCorners |= 0x04;
                if (corner.x < boundingBox.min.x && corner.y > boundingBox.max.y) inCorners |= 0x08;
            }

            // bounding box is bigger than rectangle
            if (inCorners == 0x0F)
            {
                return true;
            }

            Vector2 boundingBoxCorners[4] = {
                Vector2(boundingBox.min),
                Vector2(boundingBox.max.x, boundingBox.min.y),
                Vector2(boundingBox.max),
                Vector2(boundingBox.min.x, boundingBox.max.y)
            };

            for (uint32_t current = 0; current < edges.size(); ++current)
            {
                uint32_t next = (current == (edges.size() -1)) ? 0 : current + 1;

                if (linesIntersect(Vector2(edges[current].x, edges[current].y), Vector2(edges[next].x, edges[next].y), boundingBoxCorners[0], boundingBoxCorners[1]) || // left
                    linesIntersect(Vector2(edges[current].x, edges[current].y), Vector2(edges[next].x, edges[next].y), boundingBoxCorners[1], boundingBoxCorners[2]) || // top
                    linesIntersect(Vector2(edges[current].x, edges[current].y), Vector2(edges[next].x, edges[next].y), boundingBoxCorners[2], boundingBoxCorners[3]) || // right
                    linesIntersect(Vector2(edges[current].x, edges[current].y), Vector2(edges[next].x, edges[next].y), boundingBoxCorners[3], boundingBoxCorners[0])) // bottom
                {
                    return true;
                }
            }

            return false;
        }
    } // namespace scene
} // namespace ouzel
