// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "NativeCursor.hpp"
#include "input/InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        NativeCursor::NativeCursor(InputSystem& initInputSystem):
            inputSystem(initInputSystem)
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
    } // namespace input
} // namespace ouzel
