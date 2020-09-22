// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_COCOA_AUTORELEASEPOOL_HPP
#define OUZEL_CORE_COCOA_AUTORELEASEPOOL_HPP

#if defined(__OBJC__)
typedef NSAutoreleasePool* NSAutoreleasePoolPtr;
#else
#  include <objc/NSObjCRuntime.h>
using NSAutoreleasePoolPtr = id;
#endif

namespace ouzel::core::cocoa
{
    class AutoreleasePool final
    {
    public:
        AutoreleasePool() noexcept;
        ~AutoreleasePool();
        AutoreleasePool(const AutoreleasePool& other) noexcept;
        AutoreleasePool(AutoreleasePool&& other) noexcept;
        AutoreleasePool& operator=(const AutoreleasePool& other) noexcept;
        AutoreleasePool& operator=(AutoreleasePool&& other) noexcept;

        bool operator==(const AutoreleasePool& other) const noexcept;
        bool operator!=(const AutoreleasePool& other) const noexcept;

    private:
        NSAutoreleasePoolPtr pool = nil;
    };
}

#endif // OUZEL_CORE_COCOA_AUTORELEASEPOOL_HPP
