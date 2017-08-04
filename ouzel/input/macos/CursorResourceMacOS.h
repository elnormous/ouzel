// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(__OBJC__)
#import <AppKit/AppKit.h>
#else
#include <objc/objc.h>
typedef id NSCursor;
#endif

#include "input/CursorResource.h"

namespace ouzel
{
    namespace input
    {
        class CursorResourceMacOS: public CursorResource
        {
        public:
            virtual ~CursorResourceMacOS();

            virtual bool init(SystemCursor newSystemCursor) override;
            virtual bool init(const std::vector<uint8_t>& newData,
                              const Size2& newSize,
                              graphics::PixelFormat newPixelFormat,
                              const Vector2& newHotSpot) override;

            NSCursor* getNativeCursor() const { return cursor; }

        protected:
            NSCursor* cursor = Nil;
        };
    } // namespace input
} // namespace ouzel
