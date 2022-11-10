// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_COREFOUNDATION_POINTER_HPP
#define OUZEL_PLATFORM_COREFOUNDATION_POINTER_HPP

#include <type_traits>
#include <CoreFoundation/CoreFoundation.h>

namespace ouzel::platform::corefoundation
{
    template <class T = CFTypeRef, typename std::enable_if_t<std::is_pointer_v<T>>* = nullptr>
    class Pointer final
    {
    public:
        Pointer() noexcept = default;

        ~Pointer()
        {
            if (p) CFRelease(p);
        }

        Pointer(T a) noexcept: p{a} {}
        Pointer& operator=(T a) noexcept
        {
            if (p) CFRelease(p);
            p = a;
            return *this;
        }

        Pointer(const Pointer& other) noexcept: p{other.p}
        {
            if (p) CFRetain(p);
        }

        Pointer& operator=(const Pointer& other) noexcept
        {
            if (&other == this) return *this;
            if (other.p) CFRetain(other.p);
            if (p) CFRelease(p);
            p = other.p;
            return *this;
        }

        Pointer(Pointer&& other) noexcept: p{other.p}
        {
            other.p = nullptr;
        }

        Pointer& operator=(Pointer&& other) noexcept
        {
            if (&other == this) return *this;
            if (p) CFRelease(p);
            p = other.p;
            other.p = nullptr;
            return *this;
        }

        auto get() const noexcept { return p; }
        operator T() const noexcept { return p; }
        bool operator!() const noexcept { return p == nullptr; }

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
            return p == nullptr;
        }

        bool operator!=(std::nullptr_t) const noexcept
        {
            return p != nullptr;
        }

    private:
        T p = nullptr;
    };
}

#endif // OUZEL_PLATFORM_COREFOUNDATION_POINTER_HPP
