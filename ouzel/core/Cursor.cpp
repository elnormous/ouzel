// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Cursor.h"
#include "Application.h"

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
}
