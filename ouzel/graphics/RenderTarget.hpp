// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERTARGET_HPP
#define OUZEL_GRAPHICS_RENDERTARGET_HPP

#include <memory>
#include <vector>
#include "graphics/GraphicsResource.hpp"
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
            RenderTarget()
            {
            }
            
            explicit RenderTarget(Renderer& initRenderer);

            RenderTarget(const RenderTarget&) = delete;
            RenderTarget& operator=(const RenderTarget&) = delete;

            inline uintptr_t getResource() const { return resource.getId(); }

            void addColorTexture(const std::shared_ptr<Texture>& texture);
            void removeColorTexture(const std::shared_ptr<Texture>& texture);
            void removeColorTexture(Texture* texture);
            inline const std::vector<std::shared_ptr<Texture>>& getColorTextures() const { return colorTextures; }

            void setDepthTexture(const std::shared_ptr<Texture>& texture);
            inline const std::shared_ptr<Texture>& getDepthTexture() const { return depthTexture; }

        private:
            Resource resource;

            std::vector<std::shared_ptr<Texture>> colorTextures;
            std::shared_ptr<Texture> depthTexture;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_RENDERTARGET_HPP
