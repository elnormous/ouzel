// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <windows.h>
#include "input/CursorInterface.h"

namespace ouzel
{
    namespace input
    {
        class CursorInterfaceWin: public CursorInterface
        {
        public:
            virtual ~CursorInterfaceWin();

            virtual bool init(SystemCursor newSystemCursor) override;
            virtual bool init(const std::vector<uint8_t>& newData,
                              const Size2& newSize,
                              graphics::PixelFormat newPixelFormat,
                              const Vector2& newHotSpot) override;

            HCURSOR getNativeCursor() const { return cursor; }

        protected:
            HCURSOR cursor = nullptr;
            bool shared = false;
        };
    } // namespace input
} // namespace ouzel
