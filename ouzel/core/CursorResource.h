// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <mutex>
#include <vector>
#include "core/Cursor.h"
#include "math/Size2.h"

namespace ouzel
{
    class Application;

    class CursorResource
    {
        friend Application;
    public:
        CursorResource();

        bool init(SystemCursor newSystemCursor);
        bool init(const std::vector<uint8_t>& newData,
                  const Size2& newSize,
                  const Vector2& newHotSpot);

    protected:
        virtual bool upload();

        bool dirty = false;
        SystemCursor systemCursor = SystemCursor::DEFAULT;
        std::vector<uint8_t> data;
        Size2 size;
        Vector2 hotSpot;

        std::mutex uploadMutex;
    };
}
