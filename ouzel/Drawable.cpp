// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Drawable.h"
#include "Utils.h"

namespace ouzel
{
    namespace scene
    {
        Drawable::~Drawable()
        {
        }

        void Drawable::draw(const Matrix4& projection, const Matrix4& transform, const graphics::Color& color)
        {
            OUZEL_UNUSED(projection);
            OUZEL_UNUSED(transform);
            OUZEL_UNUSED(color);
        }

        bool Drawable::pointOn(const Vector2& position) const
        {
            return _boundingBox.containPoint(position);
        }
    } // namespace scene
} // namespace ouzel
