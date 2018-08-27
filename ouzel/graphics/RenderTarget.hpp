// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <memory>

namespace ouzel
{
    namespace graphics
    {
        class Renderer;
        class RenderTargetResource;
        class TextureResource;

        class RenderTarget final
        {
        public:
            enum Flags
            {
                COLOR_BUFFER = 0x01,
                DEPTH_BUFFER = 0x02,
                BINDABLE_COLOR_BUFFER = 0x04,
                BINDABLE_DEPTH_BUFFER = 0x08,
            };

            RenderTarget(Renderer& initRenderer);

            inline RenderTargetResource* getResource() const { return resource; }
            inline TextureResource* getColorTextureResource() const { return colorTexture; }
            inline TextureResource* getDepthTextureResource() const { return depthTexture; }

        private:
            Renderer& renderer;
            RenderTargetResource* resource = nullptr;
            TextureResource* colorTexture = nullptr;
            TextureResource* depthTexture = nullptr;
        };
    } // namespace graphics
} // namespace ouzel
