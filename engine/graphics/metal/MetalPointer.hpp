// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_METALPOINTER_HPP
#define OUZEL_GRAPHICS_METALPOINTER_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <objc/message.h>
#include <objc/objc.h>

namespace ouzel::graphics::metal
{
    inline const auto retainSel = sel_registerName("retain");
    inline const auto releaseSel = sel_registerName("release");

    template <class T>
    class Pointer final
    {
    public:
        Pointer() noexcept = default;

        Pointer(T a) noexcept: p{a} {}
        Pointer& operator=(T a) noexcept
        {
            if (p) objc_msgSend(p, releaseSel);
            p = a;
            return *this;
        }

        Pointer(const Pointer& other) noexcept: p{other.p}
        {
            if (p) objc_msgSend(p, retainSel);
        }

        Pointer& operator=(const Pointer& other) noexcept
        {
            if (&other == this) return *this;
            if (p) objc_msgSend(p, releaseSel);
            p = other.p;
            if (p) objc_msgSend(p, retainSel);
            return *this;
        }

        Pointer(Pointer&& other) noexcept: p{other.p}
        {
            other.p = nil;
        }

        Pointer& operator=(Pointer&& other) noexcept
        {
            if (&other == this) return *this;
            if (p) objc_msgSend(p, releaseSel);
            p = other.p;
            other.p = nil;
            return *this;
        }

        ~Pointer()
        {
            if (p) objc_msgSend(p, releaseSel);
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
