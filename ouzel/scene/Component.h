// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "utils/Noncopyable.h"
#include "utils/Types.h"
#include "math/AABB2.h"
#include "math/Matrix4.h"
#include "graphics/Color.h"

namespace ouzel
{
    namespace scene
    {
        class Component: public ouzel::Noncopyable
        {
        public:
            virtual ~Component();

            virtual void draw(const Matrix4& projectionMatrix,
                              const Matrix4& transformMatrix,
                              const graphics::Color& drawColor,
                              const graphics::RenderTargetPtr& renderTarget,
                              const NodePtr& currentNode);

            virtual const AABB2& getBoundingBox() const { return boundingBox; }

            virtual bool pointOn(const Vector2& position) const;
            virtual bool shapeOverlaps(const std::vector<Vector2>& edges) const;

            bool isHidden() const { return hidden; }
            virtual void setHidden(bool newHidden) { hidden = newHidden; }

        protected:
            AABB2 boundingBox;
            bool hidden = false;
        };
    } // namespace scene
} // namespace ouzel
