// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STORAGE_CFPOINTER_HPP
#define OUZEL_STORAGE_CFPOINTER_HPP

#include <CoreFoundation/CoreFoundation.h>

namespace ouzel
{
    namespace storage
    {
        template <class T>
        class CfPointer
        {
        public:
            CfPointer() noexcept = default;

            inline CfPointer(T a) noexcept: p(a) {}
            inline CfPointer& operator=(T a) noexcept
            {
                if (p) CFRelease(p);
                p = a;
                return *this;
            }

            CfPointer(const CfPointer& o) = delete;
            CfPointer& operator=(const CfPointer& o) = delete;

            inline CfPointer(CfPointer&& o) noexcept: p(o.p)
            {
                o.p = NULL;
            }

            inline CfPointer& operator=(CfPointer&& o) noexcept
            {
                if (p) CFRelease(p);
                p = o.p;
                o.p = NULL;
                return *this;
            }

            ~CfPointer()
            {
                if (p) CFRelease(p);
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
            T p = NULL;
        };
    } // namespace storage
} // namespace ouzel

#endif // OUZEL_STORAGE_CFPOINTER_HPP
