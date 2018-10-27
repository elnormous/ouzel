// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"
#if OUZEL_SUPPORTS_X11
#  include <X11/X.h>
#endif
#include "input/NativeCursor.hpp"

namespace ouzel
{
    namespace input
    {
        class NativeCursorLinux final: public NativeCursor
        {
        public:
            explicit NativeCursorLinux(InputSystem& initInputSystem);
            virtual ~NativeCursorLinux();

            virtual void init(SystemCursor newSystemCursor) override;
            virtual void init(const std::vector<uint8_t>& newData,
                              const Size2& newSize,
                              graphics::PixelFormat newPixelFormat,
                              const Vector2& newHotSpot) override;

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
