// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALPOINTER_HPP
#define OUZEL_GRAPHICS_METALPOINTER_HPP

#ifndef __OBJC__
# include <objc/message.h>
# include <objc/objc.h>
#endif

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            template <class T>
            class Pointer
            {
            public:
                Pointer() noexcept = default;

                inline Pointer(T a) noexcept: p(a) {}
                inline Pointer& operator=(T a) noexcept
                {
                    if (p)
#ifdef __OBJC__
                        [p release];
#else
                        objc_msgSend(p, sel_getUid("release"));
#endif

                    p = a;
                    return *this;
                }

                Pointer(const Pointer& o) = delete;
                Pointer& operator=(const Pointer& o) = delete;

                inline Pointer(Pointer&& o) noexcept: p(o.p)
                {
                    o.p = nil;
                }

                inline Pointer& operator=(Pointer&& o) noexcept
                {
                    if (p)
#ifdef __OBJC__
                        [p release];
#else
                        objc_msgSend(p, sel_getUid("release"));
#endif
                    p = o.p;
                    o.p = nil;
                    return *this;
                }

                ~Pointer()
                {
                    if (p)
#ifdef __OBJC__
                        [p release];
#else
                        objc_msgSend(p, sel_getUid("release"));
#endif
                }

                inline T get() const noexcept
                {
                    return p;
                }

                explicit inline operator bool() const noexcept
                {
                    return p != nil;
                }

            private:
                T p = nil;
            };
        }
    }
}

#endif // OUZEL_GRAPHICS_METALPOINTER_HPP
