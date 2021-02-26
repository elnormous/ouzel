// Ouzel by Elviss Strazdins

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

#include "../Cursor.hpp"
#include "../../math/Size.hpp"

namespace ouzel::input::windows
{
    class Cursor final
    {
    public:
        explicit Cursor(SystemCursor systemCursor);
        Cursor(const std::vector<std::uint8_t>& data,
               const Size<float, 2>& size,
               graphics::PixelFormat pixelFormat,
               const Vector<float, 2>& hotSpot);
        ~Cursor();

        Cursor(const Cursor&) = delete;
        Cursor& operator=(const Cursor&) = delete;
        Cursor(Cursor&&) = delete;
        Cursor& operator=(Cursor&&) = delete;

        auto getCursor() const noexcept { return cursor; }

    private:
        HCURSOR cursor = nullptr;
        HCURSOR ownedCursor = nullptr;
    };
}

#endif // OUZEL_INPUT_NATIVECURSORWIN_HPP
