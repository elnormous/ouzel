// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(__OBJC__)
#import <AppKit/AppKit.h>
#else
#include <objc/objc.h>
typedef id NSCursor;
#endif

#include "core/CursorResource.h"

namespace ouzel
{
    class CursorResourceMacOS: public CursorResource
    {
    public:
        virtual ~CursorResourceMacOS();
        NSCursor* getNativeCursor() const { return cursor; }

    protected:
        virtual bool upload() override;

        NSCursor* cursor = Nil;
    };
}
