// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CursorResourceWin.h"

namespace ouzel
{
    CursorResourceWin::~CursorResourceWin()
    {
    }

    bool CursorResourceWin::upload()
    {
        std::lock_guard<std::mutex> lock(uploadMutex);

        if (dirty)
        {
            dirty = false;
        }

        return true;
    }
}
