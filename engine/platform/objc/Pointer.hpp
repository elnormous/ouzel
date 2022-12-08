// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_OBJC_POINTER_HPP
#define OUZEL_PLATFORM_OBJC_POINTER_HPP

#include <type_traits>
#include <objc/message.h>
#include <objc/objc.h>

namespace ouzel::platform::objc
{
    template <class T = id, typename std::enable_if_t<std::is_pointer_v<T>>* = nullptr>
    class Pointer final
    {
        inline static const auto retainSel = sel_registerName("retain");
        inline static const auto releaseSel = sel_registerName("release");

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
            if (other.p) reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(other.p, retainSel);
            if (p) reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(p, releaseSel);
            p = other.p;
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

        [[nodiscard]] T get() const noexcept { return p; }
        [[nodiscard]] operator T() const noexcept { return p; }
        [[nodiscard]] bool operator !() const noexcept { return p == nil; }

        [[nodiscard]] bool operator==(const Pointer& other) const noexcept
        {
            return p == other.p;
        }

        [[nodiscard]] bool operator!=(const Pointer& other) const noexcept
        {
            return p != other.p;
        }

        [[nodiscard]] bool operator==(std::nullptr_t) const noexcept
        {
            return p == nil;
        }

        [[nodiscard]] bool operator!=(std::nullptr_t) const noexcept
        {
            return p != nil;
        }

    private:
        T p = nil;
    };
}

#endif // OUZEL_PLATFORM_OBJC_POINTER_HPP
