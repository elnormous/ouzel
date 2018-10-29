// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#include "input/NativeCursor.hpp"

namespace ouzel
{
    namespace input
    {
        class NativeCursorWin final: public NativeCursor
        {
        public:
            explicit NativeCursorWin(InputSystem& initInputSystem);
            virtual ~NativeCursorWin();

            void init(SystemCursor newSystemCursor) override;
            void init(const std::vector<uint8_t>& newData,
                      const Size2& newSize,
                      graphics::PixelFormat newPixelFormat,
                      const Vector2& newHotSpot) override;

            HCURSOR getNativeCursor() const { return cursor; }

        private:
            HCURSOR cursor = nullptr;
            bool shared = false;
        };
    } // namespace input
} // namespace ouzel
