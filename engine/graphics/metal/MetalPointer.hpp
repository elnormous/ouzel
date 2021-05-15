// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_METALPOINTER_HPP
#define OUZEL_GRAPHICS_METALPOINTER_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#ifndef __OBJC__
# include <objc/message.h>
# include <objc/objc.h>
#endif

namespace ouzel::graphics::metal
{
    template <class T>
    class Pointer final
    {
    public:
        Pointer() noexcept = default;

        Pointer(T a) noexcept: p{a} {}
        Pointer& operator=(T a) noexcept
        {
#ifdef __OBJC__
            [p release];
#else
            if (p) objc_msgSend(p, sel_getUid("release"));
#endif

            p = a;
            return *this;
        }

        Pointer(const Pointer& other) noexcept: p{other.p}
        {
#ifdef __OBJC__
            [p retain];
#else
            if (p) objc_msgSend(p, sel_getUid("retain"));
#endif
        }

        Pointer& operator=(const Pointer& other) noexcept
        {
            if (&other == this) return *this;
#ifdef __OBJC__
            [p release];
#else
            if (p) objc_msgSend(p, sel_getUid("release"));
#endif

            p = other.p;

#ifdef __OBJC__
            [p retain];
#else
            if (p) objc_msgSend(p, sel_getUid("retain"));
#endif
            return *this;
        }

        Pointer(Pointer&& other) noexcept: p{other.p}
        {
            other.p = nil;
        }

        Pointer& operator=(Pointer&& other) noexcept
        {
            if (&other == this) return *this;
#ifdef __OBJC__
            [p release];
#else
            if (p) objc_msgSend(p, sel_getUid("release"));
#endif
            p = other.p;
            other.p = nil;
            return *this;
        }

        ~Pointer()
        {
#ifdef __OBJC__
            [p release];
#else
            if (p) objc_msgSend(p, sel_getUid("release"));
#endif
        }

        T get() const noexcept
        {
            return p;
        }

        explicit operator bool() const noexcept
        {
            return p != nil;
        }

        bool operator==(const Pointer& other) const noexcept
        {
            return p == other.p;
        }

        bool operator!=(const Pointer& other) const noexcept
        {
            return p == other.p;
        }

    private:
        T p = nil;
    };
}

#endif

#endif // OUZEL_GRAPHICS_METALPOINTER_HPP
