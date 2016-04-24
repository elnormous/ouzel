// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Drawable.h"
#include "Utils.h"

namespace ouzel
{
    namespace scene
    {
        void Drawable::draw(const Matrix4& projection, const Matrix4& transform)
        {
            OUZEL_UNUSED(projection);
            OUZEL_UNUSED(transform);
        }

        bool Drawable::pointOn(const Vector2& position) const
        {
            return _boundingBox.containPoint(position);
        }
    } // namespace scene
} // namespace ouzel
