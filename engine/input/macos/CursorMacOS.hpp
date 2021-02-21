// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_NATIVECURSORMACOS_HPP
#define OUZEL_INPUT_NATIVECURSORMACOS_HPP

#include <cstdint>
#include <vector>

#if defined(__OBJC__)
#  import <AppKit/AppKit.h>
typedef NSCursor* NSCursorPtr;
#else
#  include <objc/objc.h>
typedef id NSCursorPtr;
#endif

#include "../Cursor.hpp"
#include "../../math/Size.hpp"

namespace ouzel::input::macos
{
    class Cursor final
    {
    public:
        explicit Cursor(SystemCursor systemCursor);
        Cursor(const std::vector<std::uint8_t>& newData,
               const Size2F& size,
               graphics::PixelFormat pixelFormat,
               const Vector2F& hotSpot);
        ~Cursor();

        Cursor(const Cursor&) = delete;
        Cursor& operator=(const Cursor&) = delete;
        Cursor(Cursor&&) = delete;
        Cursor& operator=(Cursor&&) = delete;

        auto getCursor() const noexcept { return cursor; }

    private:
        NSCursorPtr cursor = nil;
        std::vector<std::uint8_t> data;
    };
}

#endif // OUZEL_INPUT_NATIVECURSORMACOS_HPP
