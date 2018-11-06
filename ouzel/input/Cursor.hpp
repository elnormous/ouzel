// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "input/InputSystem.hpp"
#include "input/SystemCursor.hpp"
#include "graphics/PixelFormat.hpp"
#include "math/Size2.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace input
    {
        class Cursor final
        {
        public:
            explicit Cursor(InputManager& initInputManager);
            Cursor(InputManager& initInputManager, SystemCursor systemCursor);
            Cursor(InputManager& initInputManager, const std::string& filename, const Vector2& hotSpot = Vector2());
            ~Cursor();

            Cursor(const Cursor&) = delete;
            Cursor& operator=(const Cursor&) = delete;

            Cursor(Cursor&&) = delete;
            Cursor& operator=(Cursor&&) = delete;

            void init(SystemCursor systemCursor);
            void init(const std::string& filename, const Vector2& hotSpot = Vector2());
            void init(const std::vector<uint8_t>& data,
                      const Size2& size,
                      graphics::PixelFormat pixelFormat,
                      const Vector2& hotSpot);

            uint64_t getCursorResource() const { return cursorResource; }

        private:
            InputManager& inputManager;
            uintptr_t cursorResource = 0;
        };
    } // namespace input
} // namespace ouzel
