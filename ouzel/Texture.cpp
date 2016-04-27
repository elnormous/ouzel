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

        bool Texture::init(const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            size = newSize;
            dynamic = newDynamic;
            mipmaps = newMipmaps;

            return true;
        }

        bool Texture::initFromFile(const std::string& newFilename, bool newDynamic, bool newMipmaps)
        {
            filename = newFilename;
            dynamic = newDynamic;
            mipmaps = newMipmaps;

            Image image;
            if (!image.initFromFile(filename))
            {
                return false;
            }

            return initFromData(image.getData(), image.getSize(), dynamic, mipmaps);
        }

        bool Texture::initFromData(const void* data, const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            OUZEL_UNUSED(data);

            size = newSize;
            dynamic = newDynamic;
            mipmaps = newMipmaps;

            return true;
        }

        bool Texture::upload(const void* data, const Size2& newSize)
        {
            OUZEL_UNUSED(data);

            size = newSize;

            if (!dynamic)
            {
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
