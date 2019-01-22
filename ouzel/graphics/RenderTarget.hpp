// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERTARGET_HPP
#define OUZEL_GRAPHICS_RENDERTARGET_HPP

#include <memory>
#include <vector>
#include "math/Color.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;
        class Texture;

        class RenderTarget final
        {
        public:
            explicit RenderTarget(Renderer& initRenderer);
            RenderTarget(Renderer& initRenderer,
                         bool initClearColorBuffer,
                         bool initClearDepthBuffer,
                         Color initClearColor,
                         float initClearDepth);
            ~RenderTarget();

            inline uintptr_t getResource() const { return resource; }

            void addColorTexture(const std::shared_ptr<Texture>& texture);
            void removeColorTexture(const std::shared_ptr<Texture>& texture);
            void removeColorTexture(Texture* texture);
            inline const std::vector<std::shared_ptr<Texture>>& getColorTextures() const { return colorTextures; }

            void setDepthTexture(const std::shared_ptr<Texture>& texture);
            inline const std::shared_ptr<Texture>& getDepthTexture() const { return depthTexture; }

            inline const Size2<float>& getSize() const { return size; }

            inline bool getClearColorBuffer() const { return clearColorBuffer; }
            void setClearColorBuffer(bool clear);

            inline bool getClearDepthBuffer() const { return clearDepthBuffer; }
            void setClearDepthBuffer(bool clear);

            inline Color getClearColor() const { return clearColor; }
            void setClearColor(Color color);

            inline float getClearDepth() const { return clearDepth; }
            void setClearDepth(float clear);

        private:
            Renderer& renderer;
            uintptr_t resource = 0;
            std::vector<std::shared_ptr<Texture>> colorTextures;
            std::shared_ptr<Texture> depthTexture;

            Size2<float> size;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
            Color clearColor;
            float clearDepth = 1.0F;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_RENDERTARGET_HPP
