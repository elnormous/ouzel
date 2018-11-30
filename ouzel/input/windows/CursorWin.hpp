// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_NATIVECURSORWIN_HPP
#define OUZEL_INPUT_NATIVECURSORWIN_HPP

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
        class CursorWin final
        {
        public:
            explicit CursorWin(SystemCursor systemCursor);
            CursorWin(const std::vector<uint8_t>& data,
                            const Size2& size,
                            graphics::PixelFormat pixelFormat,
                            const Vector2& hotSpot);
            ~CursorWin();

            HCURSOR getCursor() const { return cursor; }

        private:
            HCURSOR cursor = nullptr;
            HCURSOR ownedCursor = nullptr;
            HDC dc = nullptr;
            HBITMAP color = nullptr;
            HBITMAP mask = nullptr;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_NATIVECURSORWIN_HPP
