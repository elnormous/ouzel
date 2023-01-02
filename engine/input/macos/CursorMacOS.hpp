// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_NATIVECURSORMACOS_HPP
#define OUZEL_INPUT_NATIVECURSORMACOS_HPP

#include <cstdint>
#include <vector>

#ifdef __OBJC__
#  import <AppKit/NSCursor.h>
using NSCursorPtr = NSCursor*;
#else
#  include <objc/objc.h>
using NSCursorPtr = id;
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
               const math::Size<std::uint32_t, 2>& size,
               graphics::PixelFormat pixelFormat,
               const math::Vector<float, 2>& hotSpot);
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
