// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Cursor.h"

namespace ouzel
{
    class Application;

    class CursorResource
    {
        friend Application;
    public:
        CursorResource();

        bool init(SystemCursor systemCursor);
        bool init(const std::string& filename, const Vector2& hotSpot);

    protected:
        virtual bool upload();
    };
}
