// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_FOUNDATION_AUTORELEASEPOOL_HPP
#define OUZEL_PLATFORM_FOUNDATION_AUTORELEASEPOOL_HPP

#include <objc/message.h>
#include <objc/objc.h>

namespace ouzel::platform::foundation
{
    inline const auto cls = objc_lookUpClass("NSAutoreleasePool");

    inline const auto allocSel = sel_registerName("alloc");
    inline const auto initSel = sel_registerName("init");
    inline const auto retainSel = sel_registerName("retain");
    inline const auto releaseSel = sel_registerName("release");
    inline const auto drainSel = sel_registerName("drain");
    
    class AutoreleasePool final
    {
    public:
        AutoreleasePool() noexcept:
            pool{reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(reinterpret_cast<id (*)(Class, SEL)>(&objc_msgSend)(cls, allocSel), initSel)}
        {
        }

        ~AutoreleasePool()
        {
            reinterpret_cast<void (*)(id, SEL)>(&objc_msgSend)(pool, releaseSel);
        }

        AutoreleasePool(const AutoreleasePool& other) noexcept:
            pool{reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(other.pool, retainSel)}
        {
        }

        AutoreleasePool(AutoreleasePool&& other) noexcept:
            pool{other.pool}
        {
            other.pool = nil;
        }

        AutoreleasePool& operator=(const AutoreleasePool& other) noexcept
        {
            if (&other == this) return *this;
            reinterpret_cast<void (*)(id, SEL)>(&objc_msgSend)(other.pool, retainSel);
            reinterpret_cast<void (*)(id, SEL)>(&objc_msgSend)(pool, releaseSel);
            pool = other.pool;
            return *this;
        }

        AutoreleasePool& operator=(AutoreleasePool&& other) noexcept
        {
            if (&other == this) return *this;
            reinterpret_cast<void (*)(id, SEL)>(&objc_msgSend)(pool, releaseSel);
            pool = other.pool;
            other.pool = nil;
            return *this;
        }

        bool operator==(const AutoreleasePool& other) const noexcept
        {
            return pool == other.pool;
        }

        bool operator!=(const AutoreleasePool& other) const noexcept
        {
            return pool != other.pool;
        }

        void drain() const noexcept
        {
            reinterpret_cast<void (*)(id, SEL)>(&objc_msgSend)(pool, drainSel);
        }

    private:
        id pool = nil;
    };
}

#endif // OUZEL_PLATFORM_FOUNDATION_AUTORELEASEPOOL_HPP
