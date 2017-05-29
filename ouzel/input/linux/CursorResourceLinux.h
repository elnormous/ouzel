// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <X11/X.h>
#include "input/CursorResource.h"

namespace ouzel
{
    namespace input
    {
        class CursorResourceLinux: public CursorResource
        {
        public:
            virtual ~CursorResourceLinux();

            ::Cursor getNativeCursor() const { return cursor; }

        protected:
            virtual bool upload() override;

            ::Cursor cursor = None;
        };
    } // namespace input
} // namespace ouzel
