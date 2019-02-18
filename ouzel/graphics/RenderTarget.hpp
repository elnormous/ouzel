// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERTARGET_HPP
#define OUZEL_GRAPHICS_RENDERTARGET_HPP

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

            explicit RenderTarget(Renderer& initRenderer,
                                  const std::vector<Texture*>& initColorTextures,
                                  Texture* initDepthTexture);

            RenderTarget(const RenderTarget&) = delete;
            RenderTarget& operator=(const RenderTarget&) = delete;

            inline uintptr_t getResource() const { return resource.getId(); }

            inline const std::vector<Texture*>& getColorTextures() const { return colorTextures; }
            inline Texture* getDepthTexture() const { return depthTexture; }

        private:
            Resource resource;

            std::vector<Texture*> colorTextures;
            Texture* depthTexture = nullptr;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_RENDERTARGET_HPP
