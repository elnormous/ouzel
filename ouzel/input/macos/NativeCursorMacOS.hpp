// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <vector>

#if defined(__OBJC__)
#  import <AppKit/AppKit.h>
typedef NSCursor* NSCursorPtr;
#else
#  include <objc/objc.h>
typedef id NSCursorPtr;
#endif

#include "input/Cursor.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    namespace input
    {
        class NativeCursorMacOS final
        {
        public:
            explicit NativeCursorMacOS(SystemCursor systemCursor);
            NativeCursorMacOS(const std::vector<uint8_t>& data,
                              const Size2& size,
                              graphics::PixelFormat pixelFormat,
                              const Vector2& hotSpot);
            ~NativeCursorMacOS();

            inline NSCursorPtr getNativeCursor() const { return cursor; }

        private:
            NSCursorPtr cursor = nil;
        };
    } // namespace input
} // namespace ouzel
