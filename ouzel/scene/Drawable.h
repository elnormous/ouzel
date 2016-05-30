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
        class Drawable: public ouzel::Noncopyable
        {
        public:
            virtual ~Drawable();

            virtual void draw(const Matrix4& projectionMatrix, const Matrix4& transformMatrix, const graphics::Color& drawColor);

            virtual const AABB2& getBoundingBox() const { return boundingBox; }

            virtual bool pointOn(const Vector2& position) const;
            virtual bool shapeOverlaps(const std::vector<Vector2>& edges) const;

            bool isVisible() const { return visible; }
            virtual void setVisible(bool newVisible) { visible = newVisible; }

            void setParentNode(const NodePtr newParentNode) { parentNode = newParentNode; }
            NodePtr getParentNode() const { return parentNode.lock(); }

        protected:
            AABB2 boundingBox;
            bool visible = true;

            NodeWeakPtr parentNode;
        };
    } // namespace scene
} // namespace ouzel
