// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CursorResourceWin.h"

namespace ouzel
{
    CursorResourceWin::~CursorResourceWin()
    {
        if (cursor && !shared) DestroyCursor(cursor);
    }

    bool CursorResourceWin::upload()
    {
        std::lock_guard<std::mutex> lock(uploadMutex);

        if (dirty)
        {
            if (cursor && !shared)
            {
                DestroyCursor(cursor);
                cursor = 0;
            }

            switch (systemCursor)
            {
            case SystemCursor::DEFAULT:
                cursor = LoadCursor(nullptr, IDC_ARROW);
                break;
            case SystemCursor::ARROW:
                cursor = LoadCursor(nullptr, IDC_ARROW);
                break;
            case SystemCursor::HAND:
                cursor = LoadCursor(nullptr, IDC_HAND);
                break;
            case SystemCursor::HORIZONTAL_RESIZE:
                cursor = LoadCursor(nullptr, IDC_SIZEWE);
                break;
            case SystemCursor::VERTICAL_RESIZE:
                cursor = LoadCursor(nullptr, IDC_SIZENS);
                break;
            case SystemCursor::CROSS:
                cursor = LoadCursor(nullptr, IDC_CROSS);
                break;
            }

            shared = true;

            dirty = false;
        }

        return true;
    }
}
