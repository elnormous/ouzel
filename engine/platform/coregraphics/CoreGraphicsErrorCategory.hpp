// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_COREGRAPHICS_COREVIDEOERRORCATEGORY_HPP
#define OUZEL_PLATFORM_COREGRAPHICS_COREVIDEOERRORCATEGORY_HPP

#include <system_error>
#include <CoreGraphics/CGError.h>

namespace ouzel::platform::coregraphics
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "CoreGraphics";
        }

        std::string message(int condition) const override
        {
            switch (condition)
            {
                case kCGErrorFailure: return "kCGErrorFailure";
                case kCGErrorIllegalArgument: return "kCGErrorIllegalArgument";
                case kCGErrorInvalidConnection: return "kCGErrorInvalidConnection";
                case kCGErrorInvalidContext: return "kCGErrorInvalidContext";
                case kCGErrorCannotComplete: return "kCGErrorCannotComplete";
                case kCGErrorNotImplemented: return "kCGErrorNotImplemented";
                case kCGErrorRangeCheck: return "kCGErrorRangeCheck";
                case kCGErrorTypeCheck: return "kCGErrorTypeCheck";
                case kCGErrorInvalidOperation: return "kCGErrorInvalidOperation";
                case kCGErrorNoneAvailable: return "kCGErrorNoneAvailable";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };

    const std::error_category& getErrorCategory() noexcept;
}

#endif // OUZEL_PLATFORM_COREGRAPHICS_COREVIDEOERRORCATEGORY_HPP
