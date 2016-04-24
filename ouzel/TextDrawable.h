// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Drawable.h"

namespace ouzel
{
    namespace scene
    {
        class TextDrawable: public Drawable
        {
        public:
            virtual void draw(const Matrix4& projection, const Matrix4& transform) override;
        };
    } // namespace scene
} // namespace ouzel
