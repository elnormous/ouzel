// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_COREVIDEO_COREVIDEOERRORCATEGORY_HPP
#define OUZEL_PLATFORM_COREVIDEO_COREVIDEOERRORCATEGORY_HPP

#include <system_error>
#include <CoreVideo/CVReturn.h>

namespace ouzel::platform::corevideo
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept final
        {
            return "CoreVideo";
        }

        std::string message(const int condition) const final
        {
            switch (condition)
            {
                case kCVReturnError: return "kCVReturnError";
                case kCVReturnInvalidArgument: return "kCVReturnInvalidArgument";
                case kCVReturnAllocationFailed: return "kCVReturnAllocationFailed";
                case kCVReturnUnsupported: return "kCVReturnUnsupported";

                    // DisplayLink related errors
                case kCVReturnInvalidDisplay: return "kCVReturnInvalidDisplay";
                case kCVReturnDisplayLinkAlreadyRunning: return "kCVReturnDisplayLinkAlreadyRunning";
                case kCVReturnDisplayLinkNotRunning: return "kCVReturnDisplayLinkNotRunning";
                case kCVReturnDisplayLinkCallbacksNotSet: return "kCVReturnDisplayLinkCallbacksNotSet";

                    // Buffer related errors
                case kCVReturnInvalidPixelFormat: return "kCVReturnInvalidPixelFormat";
                case kCVReturnInvalidSize: return "kCVReturnInvalidSize";
                case kCVReturnInvalidPixelBufferAttributes: return "kCVReturnInvalidPixelBufferAttributes";
                case kCVReturnPixelBufferNotOpenGLCompatible: return "kCVReturnPixelBufferNotOpenGLCompatible";
                case kCVReturnPixelBufferNotMetalCompatible: return "kCVReturnPixelBufferNotMetalCompatible";

                    // Buffer Pool related errors
                case kCVReturnWouldExceedAllocationThreshold: return "kCVReturnWouldExceedAllocationThreshold";
                case kCVReturnPoolAllocationFailed: return "kCVReturnPoolAllocationFailed";
                case kCVReturnInvalidPoolAttributes: return "kCVReturnInvalidPoolAttributes";
                case kCVReturnRetry: return "kCVReturnRetry";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };

    const std::error_category& getErrorCategory() noexcept;
}

#endif // OUZEL_PLATFORM_COREVIDEO_COREVIDEOERRORCATEGORY_HPP
