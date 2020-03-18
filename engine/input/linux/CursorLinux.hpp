// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_NATIVECURSORLINUX_HPP
#define OUZEL_INPUT_NATIVECURSORLINUX_HPP

#include <cstdint>
#include <vector>

#include "core/Setup.h"
#if OUZEL_SUPPORTS_X11
#  include <X11/X.h>
#endif

#include "input/Cursor.hpp"
#include "math/Size.hpp"

namespace ouzel
{
    namespace input
    {
        class CursorLinux final
        {
        public:
            explicit CursorLinux(SystemCursor systemCursor);
            CursorLinux(const std::vector<std::uint8_t>& data,
                        const Size2F& size,
                        graphics::PixelFormat pixelFormat,
                        const Vector2F& hotSpot);
            ~CursorLinux();

            CursorLinux(const CursorLinux&) = delete;
            CursorLinux& operator=(const CursorLinux&) = delete;
            CursorLinux(CursorLinux&&) = delete;
            CursorLinux& operator=(CursorLinux&&) = delete;

#if OUZEL_SUPPORTS_X11
            inline auto getCursor() const noexcept { return cursor; }
#endif

        private:
#if OUZEL_SUPPORTS_X11
            ::Cursor cursor = None;
#endif
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_NATIVECURSORLINUX_HPP
