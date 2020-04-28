// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STORAGE_CFPOINTER_HPP
#define OUZEL_STORAGE_CFPOINTER_HPP

#include <CoreFoundation/CoreFoundation.h>

namespace ouzel
{
    namespace storage
    {
        template <class T>
        class CfPointer final
        {
        public:
            CfPointer() noexcept = default;

            CfPointer(T a) noexcept: p(a) {}
            CfPointer& operator=(T a) noexcept
            {
                if (p) CFRelease(p);
                p = a;
                return *this;
            }

            CfPointer(const CfPointer& o) noexcept:
                p(o.p)
            {
                if (p) CFRetain(p);
            }

            CfPointer& operator=(const CfPointer& o) noexcept
            {
                if (p) CFRelease(p);
                p = o.p;
                if (p) CFRetain(p);
            }

            CfPointer(CfPointer&& o) noexcept: p(o.p)
            {
                o.p = nil;
            }

            CfPointer& operator=(CfPointer&& o) noexcept
            {
                if (p) CFRelease(p);
                p = o.p;
                o.p = nil;
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
    } // namespace storage
} // namespace ouzel

#endif // OUZEL_STORAGE_CFPOINTER_HPP
