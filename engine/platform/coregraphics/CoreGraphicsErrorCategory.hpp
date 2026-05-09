#ifndef FLUXION_PLATFORM_COREGRAPHICS_COREVIDEOERRORCATEGORY_HPP
#define FLUXION_PLATFORM_COREGRAPHICS_COREVIDEOERRORCATEGORY_HPP

#include <system_error>
#include <CoreGraphics/CGError.h>

namespace fluxion::platform::coregraphics
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "CoreGraphics";
        }

        std::string message(const int condition) const override
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

    inline const ErrorCategory errorCategory;
}

#endif // FLUXION_PLATFORM_COREGRAPHICS_COREVIDEOERRORCATEGORY_HPP
