// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Cursor.h"
#include "Application.h"
#include "CursorResource.h"
#include "graphics/ImageDataSTB.h"

namespace ouzel
{
    Cursor::Cursor()
    {
        resource = sharedApplication->createCursorResource();
    }

    Cursor::~Cursor()
    {
        if (resource) sharedApplication->deleteCursorResource(resource);
    }

    bool Cursor::init(SystemCursor systemCursor)
    {
        if (!resource->init(systemCursor))
        {
            return false;
        }

        sharedApplication->uploadCursorResource(resource);

        return true;
    }

    bool Cursor::init(const std::string& filename, const Vector2& hotSpot)
    {
        graphics::ImageDataSTB image;
        if (!image.initFromFile(filename))
        {
            return false;
        }

        return init(image.getData(),
                    image.getSize(),
                    hotSpot);
    }

    bool Cursor::init(const std::vector<uint8_t>& data,
                      const Size2& size,
                      const Vector2& hotSpot)
    {
        if (!resource->init(data, size, hotSpot))
        {
            return false;
        }

        sharedApplication->uploadCursorResource(resource);

        return true;
    }
}
