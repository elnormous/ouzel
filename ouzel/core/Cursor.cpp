// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Cursor.h"
#include "Application.h"
#include "CursorResource.h"

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
        return resource->init(systemCursor);
    }

    bool Cursor::init(const std::string& filename, const Vector2& hotSpot)
    {
        return resource->init(filename, hotSpot);
    }
}
