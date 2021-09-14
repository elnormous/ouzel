// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_X11_X11ERRORCATEGORY_HPP
#define OUZEL_PLATFORM_X11_X11ERRORCATEGORY_HPP

#include <system_error>
#include <X11/Xlib.h>

namespace ouzel::platform::x11
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "X11";
        }

        std::string message(const int condition) const override
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

#endif // OUZEL_PLATFORM_X11_X11ERRORCATEGORY_HPP
