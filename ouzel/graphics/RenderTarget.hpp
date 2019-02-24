// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERTARGET_HPP
#define OUZEL_GRAPHICS_RENDERTARGET_HPP

#include <memory>
#include <vector>
#include "graphics/GraphicsResource.hpp"
#include "math/Color.hpp"
#include "math/Size.hpp"

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

            RenderTarget(Renderer& initRenderer,
                         const std::vector<std::shared_ptr<Texture>>& initColorTextures,
                         const std::shared_ptr<Texture>& initDepthTexture);

            inline uintptr_t getResource() const { return resource.getId(); }

            inline const std::vector<std::shared_ptr<Texture>>& getColorTextures() const { return colorTextures; }
            inline const std::shared_ptr<Texture>& getDepthTexture() const { return depthTexture; }

        private:
            Resource resource;

            std::vector<std::shared_ptr<Texture>> colorTextures;
            std::shared_ptr<Texture> depthTexture;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_RENDERTARGET_HPP
