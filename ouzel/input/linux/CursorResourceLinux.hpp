// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
            explicit CursorResourceLinux(InputManager& initInputManager);
            virtual ~CursorResourceLinux();

            virtual void init(SystemCursor newSystemCursor) override;
            virtual void init(const std::vector<uint8_t>& newData,
                              const Size2& newSize,
                              graphics::PixelFormat newPixelFormat,
                              const Vector2& newHotSpot) override;

            ::Cursor getNativeCursor() const { return cursor; }

        private:
            ::Cursor cursor = None;
        };
    } // namespace input
} // namespace ouzel
