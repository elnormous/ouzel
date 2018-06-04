// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/RenderResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class TextureResource;

        class RenderTargetResource: public RenderResource
        {
        public:
            enum Flags
            {
                COLOR_BUFFER = 0x01,
                DEPTH_BUFFER = 0x02,
                BINDABLE_COLOR_BUFFER = 0x04,
                BINDABLE_DEPTH_BUFFER = 0x08,
            };

            TextureResource* getColorTexture() const { return colorTexture; }
            TextureResource* getDepthTexture() const { return depthTexture; }

        private:
            TextureResource* colorTexture = nullptr;
            TextureResource* depthTexture = nullptr;
        };
    } // namespace graphics
} // namespace ouzel
