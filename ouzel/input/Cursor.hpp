// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_CURSOR_HPP
#define OUZEL_INPUT_CURSOR_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "input/InputSystem.hpp"
#include "input/SystemCursor.hpp"
#include "graphics/PixelFormat.hpp"
#include "math/Size.hpp"
#include "math/Vector.hpp"

namespace ouzel
{
    namespace input
    {
        class Cursor final
        {
        public:
            explicit Cursor(InputManager& initInputManager);
            Cursor(InputManager& initInputManager, SystemCursor systemCursor);
            Cursor(InputManager& initInputManager, const std::string& filename, const Vector2F& hotSpot = Vector2F());
            ~Cursor();

            Cursor(const Cursor&) = delete;
            Cursor& operator=(const Cursor&) = delete;

            Cursor(Cursor&&) = delete;
            Cursor& operator=(Cursor&&) = delete;

            void init(SystemCursor systemCursor);
            void init(const std::string& filename, const Vector2F& hotSpot = Vector2F());
            void init(const std::vector<uint8_t>& data,
                      const Size2F& size,
                      graphics::PixelFormat pixelFormat,
                      const Vector2F& hotSpot);

            uintptr_t getCursorResource() const { return cursorResource; }

        private:
            InputManager& inputManager;
            uintptr_t cursorResource = 0;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_CURSOR_HPP
