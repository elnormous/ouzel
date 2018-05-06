// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <X11/X.h>
#include "input/CursorResource.hpp"

namespace ouzel
{
    namespace input
    {
        class CursorResourceLinux: public CursorResource
        {
        public:
            explicit CursorResourceLinux(Input& initInput);
            virtual ~CursorResourceLinux();

            virtual bool init(SystemCursor newSystemCursor) override;
            virtual bool init(const std::vector<uint8_t>& newData,
                              const Size2& newSize,
                              graphics::PixelFormat newPixelFormat,
                              const Vector2& newHotSpot) override;

            ::Cursor getNativeCursor() const { return cursor; }

        private:
            ::Cursor cursor = None;
        };
    } // namespace input
} // namespace ouzel
