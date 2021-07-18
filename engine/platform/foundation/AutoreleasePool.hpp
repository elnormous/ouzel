// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_FOUNDATION_AUTORELEASEPOOL_HPP
#define OUZEL_PLATFORM_FOUNDATION_AUTORELEASEPOOL_HPP

#ifdef __OBJC__
#  import <Foundation/NSAutoreleasePool.h>
typedef NSAutoreleasePool* NSAutoreleasePoolPtr;
#else
#  include <objc/NSObjCRuntime.h>
using NSAutoreleasePoolPtr = id;
#endif

namespace ouzel::platform::foundation
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

        bool operator==(const AutoreleasePool& other) const noexcept
        {
            return pool == other.pool;
        }

        bool operator!=(const AutoreleasePool& other) const noexcept
        {
            return pool != other.pool;
        }

    private:
        NSAutoreleasePoolPtr pool = nil;
    };
}

#endif // OUZEL_PLATFORM_FOUNDATION_AUTORELEASEPOOL_HPP
