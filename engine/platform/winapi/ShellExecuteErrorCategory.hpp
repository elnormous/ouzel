// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_WINAPI_SHELLEXECUTEERRORCATEGORY_HPP
#define OUZEL_PLATFORM_WINAPI_SHELLEXECUTEERRORCATEGORY_HPP

#include <system_error>

#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <winerror.h>
#include <shellapi.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

namespace ouzel::platform::winapi
{
    class ShellExecuteErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "ShellExecute";
        }

        std::string message(const int condition) const override
        {
            switch (condition)
            {
                case 0: return "Out of memory";
                case ERROR_FILE_NOT_FOUND: return "ERROR_FILE_NOT_FOUND";
                case ERROR_PATH_NOT_FOUND: return "ERROR_PATH_NOT_FOUND";
                case ERROR_BAD_FORMAT: return "ERROR_BAD_FORMAT";
                case SE_ERR_ACCESSDENIED: return "SE_ERR_ACCESSDENIED";
                case SE_ERR_ASSOCINCOMPLETE: return "SE_ERR_ASSOCINCOMPLETE";
                case SE_ERR_DDEBUSY: return "SE_ERR_DDEBUSY";
                case SE_ERR_DDEFAIL: return "SE_ERR_DDEFAIL";
                case SE_ERR_DDETIMEOUT: return "SE_ERR_DDETIMEOUT";
                case SE_ERR_DLLNOTFOUND: return "SE_ERR_DLLNOTFOUND";
                case SE_ERR_NOASSOC: return "SE_ERR_NOASSOC";
                case SE_ERR_OOM: return "SE_ERR_OOM";
                case SE_ERR_SHARE: return "SE_ERR_SHARE";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };
}

#endif // OUZEL_PLATFORM_WINAPI_SHELLEXECUTEERRORCATEGORY_HPP
