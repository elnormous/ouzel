// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CursorResourceWin.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace input
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
                if (cursor)
                {
                    if (!shared) DestroyCursor(cursor);
                    cursor = 0;
                }

                if (data.empty())
                {
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
                        case SystemCursor::I_BEAM:
                            cursor = LoadCursor(nullptr, IDC_IBEAM);
                            break;
                    }

                    if (!cursor)
                    {
                        Log(Log::Level::ERR) << "Failed to load cursor";
                        return false;
                    }

                    shared = true;
                }
                else
                {

                }

                dirty = false;
            }

            return true;
        }
    } // namespace input
} // namespace ouzel
