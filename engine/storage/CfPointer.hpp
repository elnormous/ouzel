// Ouzel by Elviss Strazdins

#ifndef OUZEL_STORAGE_CFPOINTER_HPP
#define OUZEL_STORAGE_CFPOINTER_HPP

#include <CoreFoundation/CoreFoundation.h>

namespace ouzel::storage
{
    template <class T>
    class CfPointer final
    {
    public:
        CfPointer() noexcept = default;

        CfPointer(T a) noexcept: p{a} {}
        CfPointer& operator=(T a) noexcept
        {
            if (p) CFRelease(p);
            p = a;
            return *this;
        }

        CfPointer(const CfPointer& other) noexcept: p{other.p}
        {
            if (p) CFRetain(p);
        }

        CfPointer& operator=(const CfPointer& other) noexcept
        {
            if (&other == this) return *this;
            if (p) CFRelease(p);
            p = other.p;
            if (p) CFRetain(p);
            return *this;
        }

        CfPointer(CfPointer&& other) noexcept: p{other.p}
        {
            other.p = nil;
        }

        CfPointer& operator=(CfPointer&& other) noexcept
        {
            if (&other == this) return *this;
            if (p) CFRelease(p);
            p = other.p;
            other.p = nil;
            return *this;
        }

        ~CfPointer()
        {
            if (p) CFRelease(p);
        }

        T get() const noexcept
        {
            return p;
        }

        explicit operator bool() const noexcept
        {
            return p != nil;
        }

    private:
        T p = nil;
    };
}

#endif // OUZEL_STORAGE_CFPOINTER_HPP
