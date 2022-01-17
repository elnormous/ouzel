// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_OBJC_POINTER_HPP
#define OUZEL_PLATFORM_OBJC_POINTER_HPP

#include <objc/message.h>
#include <objc/objc.h>

namespace ouzel::platform::objc
{
    inline const auto retainSel = sel_registerName("retain");
    inline const auto releaseSel = sel_registerName("release");

    template <class T = id>
    class Pointer final
    {
    public:
        Pointer() noexcept = default;

        Pointer(T a) noexcept: p{a} {}
        Pointer& operator=(T a) noexcept
        {
            if (p) reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(p, releaseSel);
            p = a;
            return *this;
        }

        Pointer(const Pointer& other) noexcept: p{other.p}
        {
            if (p) reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(p, retainSel);
        }

        Pointer& operator=(const Pointer& other) noexcept
        {
            if (&other == this) return *this;
            if (p) reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(p, releaseSel);
            p = other.p;
            if (p) reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(p, retainSel);
            return *this;
        }

        Pointer(Pointer&& other) noexcept: p{other.p}
        {
            other.p = nil;
        }

        Pointer& operator=(Pointer&& other) noexcept
        {
            if (&other == this) return *this;
            if (p) reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(p, releaseSel);
            p = other.p;
            other.p = nil;
            return *this;
        }

        ~Pointer()
        {
            if (p) reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(p, releaseSel);
        }

        T get() const noexcept
        {
            return p;
        }

        operator T() const noexcept
        {
            return p;
        }

        bool operator==(const Pointer& other) const noexcept
        {
            return p == other.p;
        }

        bool operator!=(const Pointer& other) const noexcept
        {
            return p != other.p;
        }

        bool operator==(std::nullptr_t) const noexcept
        {
            return p == nil;
        }

        bool operator!=(std::nullptr_t) const noexcept
        {
            return p != nil;
        }

    private:
        T p = nil;
    };
}

#endif // OUZEL_PLATFORM_OBJC_POINTER_HPP
