// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

namespace ouzel
{
    namespace graphics
    {
        class Renderer;
        class Texture;

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

            Texture* getColorTexture() const { return colorTexture; }
            Texture* getDepthTexture() const { return depthTexture; }

        private:
            Renderer& renderer;
            Texture* colorTexture = nullptr;
            Texture* depthTexture = nullptr;
        };
    } // namespace graphics
} // namespace ouzel
