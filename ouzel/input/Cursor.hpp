// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "graphics/PixelFormat.hpp"
#include "math/Size2.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace input
    {
        class CursorResource;

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

        class Cursor
        {
        public:
            Cursor();
            explicit Cursor(SystemCursor systemCursor);
            explicit Cursor(const std::string& filename, const Vector2& hotSpot = Vector2());
            virtual ~Cursor();

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

            inline CursorResource* getResource() const { return resource; }

        private:
            CursorResource* resource = nullptr;
        };
    } // namespace input
} // namespace ouzel
