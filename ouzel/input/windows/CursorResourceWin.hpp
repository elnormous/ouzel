// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <Windows.h>
#include "input/CursorResource.hpp"

namespace ouzel
{
    namespace input
    {
        class CursorResourceWin: public CursorResource
        {
        public:
            explicit CursorResourceWin(InputManager& initInputManager);
            virtual ~CursorResourceWin();

            virtual void init(SystemCursor newSystemCursor) override;
            virtual void init(const std::vector<uint8_t>& newData,
                              const Size2& newSize,
                              graphics::PixelFormat newPixelFormat,
                              const Vector2& newHotSpot) override;

            HCURSOR getNativeCursor() const { return cursor; }

        private:
            HCURSOR cursor = nullptr;
            bool shared = false;
        };
    } // namespace input
} // namespace ouzel
