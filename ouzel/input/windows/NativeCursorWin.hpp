// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <vector>

#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>

#include "input/Cursor.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    namespace input
    {
        class NativeCursorWin final
        {
        public:
            explicit NativeCursorWin();
            ~NativeCursorWin();

            void init(SystemCursor systemCursor);
            void init(const std::vector<uint8_t>& data,
                      const Size2& size,
                      graphics::PixelFormat pixelFormat,
                      const Vector2& hotSpot);

            HCURSOR getNativeCursor() const { return cursor; }

        private:
            HCURSOR cursor = nullptr;
            bool shared = false;
        };
    } // namespace input
} // namespace ouzel
