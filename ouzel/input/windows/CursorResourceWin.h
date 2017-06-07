// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <windows.h>
#include "input/CursorResource.h"

namespace ouzel
{
    namespace input
    {
        class CursorResourceWin: public CursorResource
        {
        public:
            virtual ~CursorResourceWin();

            HCURSOR getNativeCursor() const { return cursor; }

        protected:
            virtual bool upload() override;

            HCURSOR cursor = nullptr;
            bool shared = false;
        };
    } // namespace input
} // namespace ouzel
