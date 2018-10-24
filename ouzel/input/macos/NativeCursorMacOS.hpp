// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#if defined(__OBJC__)
#import <AppKit/AppKit.h>
#else
#include <objc/objc.h>
typedef id NSCursor;
#endif

#include "input/NativeCursor.hpp"

namespace ouzel
{
    namespace input
    {
        class NativeCursorMacOS: public NativeCursor
        {
        public:
            explicit NativeCursorMacOS(InputSystem& initInputSystem);
            virtual ~NativeCursorMacOS();

            virtual void init(SystemCursor newSystemCursor) override;
            virtual void init(const std::vector<uint8_t>& newData,
                              const Size2& newSize,
                              graphics::PixelFormat newPixelFormat,
                              const Vector2& newHotSpot) override;

            inline NSCursor* getNativeCursor() const { return cursor; }

        private:
            NSCursor* cursor = nil;
        };
    } // namespace input
} // namespace ouzel
