// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_NATIVECURSORLINUX_HPP
#define OUZEL_INPUT_NATIVECURSORLINUX_HPP

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
        class CursorLinux final
        {
        public:
            explicit CursorLinux(SystemCursor systemCursor);
            CursorLinux(const std::vector<uint8_t>& data,
                              const Size2<float>& size,
                              graphics::PixelFormat pixelFormat,
                              const Vector2<float>& hotSpot);
            ~CursorLinux();

#if OUZEL_SUPPORTS_X11
            ::Cursor getCursor() const { return cursor; }
#endif

        private:
#if OUZEL_SUPPORTS_X11
            ::Cursor cursor = None;
#endif
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_NATIVECURSORLINUX_HPP
