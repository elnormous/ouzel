// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERTARGET_HPP
#define OUZEL_GRAPHICS_RENDERTARGET_HPP

#include <memory>
#include <vector>
#include "RenderDevice.hpp"
#include "../math/Color.hpp"
#include "../math/Size.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;
        class Texture;

        class RenderTarget final
        {
        public:
            RenderTarget() = default;

            RenderTarget(Renderer& initRenderer,
                         const std::vector<Texture*>& initColorTextures,
                         Texture* initDepthTexture);

            inline auto& getResource() const noexcept { return resource; }

            inline auto& getColorTextures() const noexcept { return colorTextures; }
            inline auto getDepthTexture() const noexcept { return depthTexture; }

        private:
            RenderDevice::Resource resource;

            std::vector<Texture*> colorTextures;
            Texture* depthTexture = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_RENDERTARGET_HPP
