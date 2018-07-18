// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "LoaderImage.hpp"
#include "Cache.hpp"
#include "core/Engine.hpp"
#include "graphics/ImageDataSTB.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderImage::LoaderImage():
            Loader(TYPE, {"jpg", "jpeg", "png", "bmp", "tga"})
        {
        }

        bool LoaderImage::loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps)
        {
            graphics::ImageDataSTB image(data);

            std::shared_ptr<graphics::Texture> texture = std::make_shared<graphics::Texture>(engine->getRenderer());
            texture->init(image.getData(), image.getSize(), 0, mipmaps ? 0 : 1, image.getPixelFormat());

            cache->setTexture(filename, texture);

            return true;
        }
    } // namespace assets
} // namespace ouzel
