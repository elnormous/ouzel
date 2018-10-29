// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <memory>
#include "math/Color.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;
        class RenderResource;

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

            explicit RenderTarget(Renderer& initRenderer);

            inline RenderResource* getResource() const { return resource; }
            inline RenderResource* getColorTextureResource() const { return colorTexture; }
            inline RenderResource* getDepthTextureResource() const { return depthTexture; }

            inline const Size2& getSize() const { return size; }

            inline bool getClearColorBuffer() const { return clearColorBuffer; }
            virtual void setClearColorBuffer(bool clear);

            inline bool getClearDepthBuffer() const { return clearDepthBuffer; }
            virtual void setClearDepthBuffer(bool clear);

            inline Color getClearColor() const { return clearColor; }
            virtual void setClearColor(Color color);

            inline float getClearDepth() const { return clearDepth; }
            virtual void setClearDepth(float clear);

        private:
            Renderer& renderer;
            RenderResource* resource = nullptr;
            RenderResource* colorTexture = nullptr;
            RenderResource* depthTexture = nullptr;

            Size2 size;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
            Color clearColor;
            float clearDepth = 1.0F;
        };
    } // namespace graphics
} // namespace ouzel
