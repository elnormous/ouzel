// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_OBJC_HPP
#define OUZEL_PLATFORM_OBJC_HPP

#ifndef __OBJC__
# include <objc/message.h>
# include <objc/objc.h>
#endif

namespace ouzel::platform::objc
{
    template <class T>
    class Object final
    {
    public:
        Object() noexcept = default;

        Object(T a) noexcept: p{a} {}
        Object& operator=(T a) noexcept
        {
#ifdef __OBJC__
            [p release];
#else
            if (p) objc_msgSend(p, sel_getUid("release"));
#endif

            p = a;
            return *this;
        }

        Object(const Object& other) noexcept: p{other.p}
        {
#ifdef __OBJC__
            [p retain];
#else
            if (p) objc_msgSend(p, sel_getUid("retain"));
#endif
        }

        Object& operator=(const Object& other) noexcept
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

        Object(Object&& other) noexcept: p{other.p}
        {
            other.p = nil;
        }

        Object& operator=(Object&& other) noexcept
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

        ~Object()
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
