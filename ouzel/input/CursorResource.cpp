// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

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
            if (inputManager.currentCursorResource == this)
                inputManager.activateCursorResource(this);
        }
    } // namespace input
} // namespace ouzel
