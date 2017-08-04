// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "utils/Noncopyable.h"
#include "graphics/PixelFormat.h"
#include "math/Size2.h"
#include "math/Vector2.h"

namespace ouzel
{
    namespace input
    {
        class CursorInterface;

        enum class SystemCursor
        {
            DEFAULT,
            ARROW,
            HAND,
            HORIZONTAL_RESIZE,
            VERTICAL_RESIZE,
            CROSS,
            I_BEAM
        };

        class Cursor: public Noncopyable
        {
        public:
            Cursor();
            Cursor(SystemCursor systemCursor);
            Cursor(const std::string& filename, const Vector2& hotSpot);
            virtual ~Cursor();

            bool init(SystemCursor systemCursor);
            bool init(const std::string& filename, const Vector2& hotSpot);
            bool init(const std::vector<uint8_t>& data,
                      const Size2& size,
                      graphics::PixelFormat pixelFormat,
                      const Vector2& hotSpot);

            CursorInterface* getResource() const { return resource; }

        private:
            CursorInterface* resource = nullptr;
        };
    } // namespace input
} // namespace ouzel
