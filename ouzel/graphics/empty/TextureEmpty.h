// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/TextureResource.h"

namespace ouzel
{
    namespace graphics
    {
        class TextureEmpty: public TextureResource
        {
        public:
            TextureEmpty();

            virtual bool upload() override;
        };
    } // namespace graphics
} // namespace ouzel
