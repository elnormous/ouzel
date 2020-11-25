// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_X11ERRORCATEGORY_HPP
#define OUZEL_CORE_X11ERRORCATEGORY_HPP

#include "../Setup.h"
#if OUZEL_SUPPORTS_X11
#include <system_error>
#include <X11/Xlib.h>

namespace ouzel::core::linux
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept final
        {
            return "X11";
        }

        std::string message(int condition) const final
        {
            switch (condition)
            {
                case BadRequest: return "BadRequest";
                case BadValue: return "BadValue";
                case BadWindow: return "BadWindow";
                case BadPixmap: return "BadPixmap";
                case BadAtom: return "BadAtom";
                case BadCursor: return "BadCursor";
                case BadFont: return "BadFont";
                case BadMatch: return "BadMatch";
                case BadDrawable: return "BadDrawable";
                case BadAccess: return "BadAccess";
                case BadAlloc: return "BadAlloc";
                case BadColor: return "BadColor";
                case BadGC: return "BadGC";
                case BadIDChoice: return "BadIDChoice";
                case BadName: return "BadName";
                case BadLength: return "BadLength";
                case BadImplementation: return "BadImplementation";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };
}
#endif

#endif // OUZEL_CORE_X11ERRORCATEGORY_HPP
