// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "CursorResource.hpp"
#include "input/InputManager.hpp"

namespace ouzel
{
    namespace input
    {
        CursorResource::CursorResource(InputManager& initInputManager):
            inputManager(initInputManager)
        {
        }

        CursorResource::~CursorResource()
        {
        }

        void CursorResource::init(SystemCursor newSystemCursor)
        {
            systemCursor = newSystemCursor;
        }

        void CursorResource::init(const std::vector<uint8_t>& newData,
                                  const Size2& newSize,
                                  graphics::PixelFormat newPixelFormat,
                                  const Vector2& newHotSpot)
        {
            data = newData;
            size = newSize;
            pixelFormat = newPixelFormat;
            hotSpot = newHotSpot;
        }

        void CursorResource::reactivate()
        {
            if (inputManager.currentCursorResource == this)
                inputManager.activateCursorResource(this);
        }
    } // namespace input
} // namespace ouzel
