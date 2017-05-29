// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CursorResourceMacOS.h"

namespace ouzel
{
    namespace input
    {
        CursorResourceMacOS::~CursorResourceMacOS()
        {
            if (cursor) [cursor release];
        }

        bool CursorResourceMacOS::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
                if (data.empty())
                {
                    if (cursor)
                    {
                        [cursor release];
                        cursor = Nil;
                    }

                    switch (systemCursor)
                    {
                        case SystemCursor::DEFAULT:
                            cursor = [NSCursor currentSystemCursor];
                            break;
                        case SystemCursor::ARROW:
                            cursor = [NSCursor arrowCursor];
                            break;
                        case SystemCursor::HAND:
                            cursor = [NSCursor openHandCursor];
                            break;
                        case SystemCursor::HORIZONTAL_RESIZE:
                            cursor = [NSCursor resizeLeftRightCursor];
                            break;
                        case SystemCursor::VERTICAL_RESIZE:
                            cursor = [NSCursor resizeUpDownCursor];
                            break;
                        case SystemCursor::CROSS:
                            cursor = [NSCursor crosshairCursor];
                            break;
                    }

                    if (cursor) [cursor retain];
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
