// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Noncopyable.h"
#include "Types.h"
#include "AABB2.h"
#include "Matrix4.h"

namespace ouzel
{
    namespace scene
    {
        class Drawable: public ouzel::Noncopyable
        {
        public:
            virtual ~Drawable();

            virtual void draw(const Matrix4& projection, const Matrix4& transform);

            virtual const AABB2& getBoundingBox() const { return _boundingBox; }

            virtual bool pointOn(const Vector2& position) const;

            bool isVisible() const { return _visible; }
            virtual void setVisible(bool visible) { _visible = visible; }

            void setParentNode(const NodePtr parentNode) { _parentNode = parentNode; }
            NodePtr getParentNode() const { return _parentNode.lock(); }

        protected:
            AABB2 _boundingBox;
            bool _visible = true;

            NodeWeakPtr _parentNode;
        };
    } // namespace scene
} // namespace ouzel
