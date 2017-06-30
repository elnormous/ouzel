// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Cursor.h"
#include "Input.h"
#include "CursorResource.h"
#include "core/Engine.h"
#include "graphics/ImageDataSTB.h"

namespace ouzel
{
    namespace input
    {
        Cursor::Cursor()
        {
            resource = sharedEngine->getInput()->createCursorResource();
        }

        Cursor::Cursor(SystemCursor systemCursor):
            Cursor()
        {
            init(systemCursor);
        }

        Cursor::Cursor(const std::string& filename, const Vector2& hotSpot):
            Cursor()
        {
            init(filename, hotSpot);
        }
        
        Cursor::~Cursor()
        {
            if (sharedEngine && resource) sharedEngine->getInput()->deleteCursorResource(resource);
        }

        bool Cursor::init(SystemCursor systemCursor)
        {
            if (!resource->init(systemCursor))
            {
                return false;
            }

            sharedEngine->getInput()->uploadCursorResource(resource);

            return true;
        }

        bool Cursor::init(const std::string& filename, const Vector2& hotSpot)
        {
            graphics::ImageDataSTB image;
            if (!image.init(filename))
            {
                return false;
            }

            return init(image.getData(),
                        image.getSize(),
                        image.getPixelFormat(),
                        hotSpot);
        }

        bool Cursor::init(const std::vector<uint8_t>& data,
                          const Size2& size,
                          graphics::PixelFormat pixelFormat,
                          const Vector2& hotSpot)
        {
            if (!resource->init(data, size, pixelFormat, hotSpot))
            {
                return false;
            }

            sharedEngine->getInput()->uploadCursorResource(resource);

            return true;
        }
    } // namespace input
} // namespace ouzel
