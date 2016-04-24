// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Noncopyable.h"
#include "AABB2.h"
#include "Matrix4.h"

namespace ouzel
{
    namespace scene
    {
        class Drawable: public ouzel::Noncopyable
        {
        public:
            virtual void draw(const Matrix4& projection, const Matrix4& transform);

            virtual const AABB2& getBoundingBox() const { return _boundingBox; }
            
        protected:
            AABB2 _boundingBox;
        };
    } // namespace scene
} // namespace ouzel
