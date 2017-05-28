// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <windows.h>
#include "core/CursorResource.h"

namespace ouzel
{
    class CursorResourceWin: public CursorResource
    {
    public:
        virtual ~CursorResourceWin();

        HCURSOR getNativeCursor() const { return cursor; }

    protected:
        virtual bool upload() override;

        HCURSOR cursor = 0;
    };
}
