// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_NATIVECURSORWIN_HPP
#define OUZEL_INPUT_NATIVECURSORWIN_HPP

#include <cstdint>
#include <vector>
#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "input/Cursor.hpp"
#include "math/Size.hpp"

namespace ouzel
{
    namespace input
    {
        class CursorWin final
        {
        public:
            explicit CursorWin(SystemCursor systemCursor);
            CursorWin(const std::vector<std::uint8_t>& data,
                      const Size2F& size,
                      graphics::PixelFormat pixelFormat,
                      const Vector2F& hotSpot);
            ~CursorWin();

            CursorWin(const CursorWin&) = delete;
            CursorWin& operator=(const CursorWin&) = delete;
            CursorWin(CursorWin&&) = delete;
            CursorWin& operator=(CursorWin&&) = delete;

            inline auto getCursor() const noexcept { return cursor; }

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
