// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <vector>
#include "input/Cursor.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    namespace input
    {
        class InputManager;

        class CursorResource
        {
        public:
            explicit CursorResource(InputManager& initInputManager);
            virtual ~CursorResource();

            virtual void init(SystemCursor newSystemCursor);
            virtual void init(const std::vector<uint8_t>& newData,
                              const Size2& newSize,
                              graphics::PixelFormat newPixelFormat,
                              const Vector2& newHotSpot);

        protected:
            void reactivate();

            InputManager& inputManager;
            SystemCursor systemCursor = SystemCursor::DEFAULT;
            std::vector<uint8_t> data;
            Size2 size;
            graphics::PixelFormat pixelFormat;
            Vector2 hotSpot;
        };
    } // namespace input
} // namespace ouzel
