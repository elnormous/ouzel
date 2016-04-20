// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Texture.h"
#include "Engine.h"
#include "Renderer.h"
#include "Image.h"
#include "Utils.h"

namespace ouzel
{
    namespace graphics
    {
        Texture::Texture()
        {

        }

        Texture::~Texture()
        {

        }

        bool Texture::init(const Size2& size, bool dynamic, bool mipmaps)
        {
            _size = size;
            _dynamic = dynamic;
            _mipmaps = mipmaps;

            return true;
        }

        bool Texture::initFromFile(const std::string& filename, bool dynamic, bool mipmaps)
        {
            _filename = filename;
            _dynamic = dynamic;
            _mipmaps = mipmaps;

            Image image;
            if (!image.initFromFile(filename))
            {
                return false;
            }

            return initFromData(image.getData(), image.getSize(), dynamic, mipmaps);
        }

        bool Texture::initFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps)
        {
            OUZEL_UNUSED(data);

            _size = size;
            _dynamic = dynamic;
            _mipmaps = mipmaps;

            return true;
        }

        bool Texture::upload(const void* data, const Size2& size)
        {
            OUZEL_UNUSED(data);

            _size = size;

            if (!_dynamic)
            {
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
