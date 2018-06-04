// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    namespace graphics
    {
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

            Texture* getColorTexture() const { return colorTexture; }
            Texture* getDepthTexture() const { return depthTexture; }

        private:
            Texture* colorTexture = nullptr;
            Texture* depthTexture = nullptr;
        };
    } // namespace graphics
} // namespace ouzel
