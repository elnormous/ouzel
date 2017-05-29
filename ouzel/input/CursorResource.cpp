// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CursorResource.h"
#include "Application.h"

namespace ouzel
{
    namespace input
    {
        CursorResource::CursorResource()
        {
        }

        bool CursorResource::init(SystemCursor newSystemCursor)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            systemCursor = newSystemCursor;
            dirty = true;

            return true;
        }

        bool CursorResource::init(const std::vector<uint8_t>& newData,
                                  const Size2& newSize,
                                  const Vector2& newHotSpot)
        {
            {
                std::lock_guard<std::mutex> lock(uploadMutex);

                data = newData;
                size = newSize;
                hotSpot = newHotSpot;
                dirty = true;
            }

            return true;
        }

        bool CursorResource::upload()
        {
            return true;
        }
    } // namespace input
} // namespace ouzel
