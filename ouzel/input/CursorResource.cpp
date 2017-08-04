// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CursorResource.h"
#include "input/Input.h"
#include "core/Engine.h"

namespace ouzel
{
    namespace input
    {
        CursorResource::CursorResource()
        {
        }

        bool CursorResource::init(SystemCursor newSystemCursor)
        {
            systemCursor = newSystemCursor;

            return true;
        }

        bool CursorResource::init(const std::vector<uint8_t>& newData,
                                  const Size2& newSize,
                                  graphics::PixelFormat newPixelFormat,
                                  const Vector2& newHotSpot)
        {
            data = newData;
            size = newSize;
            pixelFormat = newPixelFormat;
            hotSpot = newHotSpot;

            return true;
        }


        void CursorResource::reactivate()
        {
            if (sharedEngine->getInput()->currentCursorResource == this)
            {
                sharedEngine->getInput()->activateCursorResource(this);
            }
        }
    } // namespace input
} // namespace ouzel
