// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CursorResource.h"

namespace ouzel
{
    CursorResource::CursorResource()
    {
    }

    bool CursorResource::init(SystemCursor systemCursor)
    {
        return true;
    }

    bool CursorResource::init(const std::string& filename, const Vector2& hotSpot)
    {
        return true;
    }

    bool CursorResource::upload()
    {
        return true;
    }
}
