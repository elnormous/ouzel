// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <vector>

#include "core/Setup.h"
#if OUZEL_SUPPORTS_X11
#  include <X11/X.h>
#endif

#include "input/Cursor.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    namespace input
    {
        class NativeCursorLinux final
        {
        public:
            explicit NativeCursorLinux(SystemCursor systemCursor);
            NativeCursorLinux(const std::vector<uint8_t>& data,
                              const Size2& size,
                              graphics::PixelFormat pixelFormat,
                              const Vector2& hotSpot);
            ~NativeCursorLinux();

#if OUZEL_SUPPORTS_X11
            ::Cursor getNativeCursor() const { return cursor; }
#endif

        private:
#if OUZEL_SUPPORTS_X11
            ::Cursor cursor = None;
#endif
        };
    } // namespace input
} // namespace ouzel
