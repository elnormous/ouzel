// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_OBJC_HPP
#define OUZEL_PLATFORM_OBJC_HPP

#include <objc/message.h>
#include <objc/objc.h>

namespace ouzel::platform::objc
{
    inline const auto retainSel = sel_registerName("retain");
    inline const auto releaseSel = sel_registerName("release");

    template <class T>
    class Object final
    {
    public:
        Object() noexcept = default;

        Object(T a) noexcept: p{a} {}
        Object& operator=(T a) noexcept
        {
            if (p) reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(p, releaseSel);
            p = a;
            return *this;
        }

        Object(const Object& other) noexcept: p{other.p}
        {
            if (p) reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(p, retainSel);
        }

        Object& operator=(const Object& other) noexcept
        {
            if (&other == this) return *this;
            if (p) reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(p, releaseSel);
            p = other.p;
            if (p) reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(p, retainSel);
            return *this;
        }

        Object(Object&& other) noexcept: p{other.p}
        {
            other.p = nil;
        }

        Object& operator=(Object&& other) noexcept
        {
            if (&other == this) return *this;
            if (p) reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(p, releaseSel);
            p = other.p;
            other.p = nil;
            return *this;
        }

        ~Object()
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

        bool operator==(const Object& other) const noexcept
        {
            return p == other.p;
        }

        bool operator!=(const Object& other) const noexcept
        {
            return p != other.p;
        }

    private:
        T p = nil;
    };
}

#endif // OUZEL_PLATFORM_OBJC_HPP
