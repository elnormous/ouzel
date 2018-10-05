// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "NativeCursor.hpp"
#include "input/InputManager.hpp"

namespace ouzel
{
    namespace input
    {
        NativeCursor::NativeCursor(InputManager& initInputManager):
            inputManager(initInputManager)
        {
        }

        NativeCursor::~NativeCursor()
        {
        }

        void NativeCursor::init(SystemCursor newSystemCursor)
        {
            systemCursor = newSystemCursor;
        }

        void NativeCursor::init(const std::vector<uint8_t>& newData,
                                  const Size2& newSize,
                                  graphics::PixelFormat newPixelFormat,
                                  const Vector2& newHotSpot)
        {
            data = newData;
            size = newSize;
            pixelFormat = newPixelFormat;
            hotSpot = newHotSpot;
        }

        void NativeCursor::reactivate()
        {
            //if (inputManager.currentNativeCursor == this)
            //    inputManager.activateNativeCursor(this);
        }
    } // namespace input
} // namespace ouzel
