// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_COREVIDEO_DISPLAYLINK_HPP
#define OUZEL_PLATFORM_COREVIDEO_DISPLAYLINK_HPP

#include <system_error>
#include <CoreVideo/CVDisplayLink.h>
#include "CoreVideoErrorCategory.hpp"

namespace ouzel::platform::corevideo
{
    class DisplayLink final
    {
    public:
        explicit DisplayLink(const CGDirectDisplayID displayId)
        {
            if (const auto result = CVDisplayLinkCreateWithCGDisplay(displayId, &displayLink); result != kCVReturnSuccess)
                throw std::system_error{result, errorCategory, "Failed to create display link"};
        }

        ~DisplayLink()
        {
            CVDisplayLinkRelease(displayLink);
        }

        DisplayLink(DisplayLink&& other) noexcept:
            displayLink{other.displayLink}
        {
            other.displayLink = nullptr;
        }

        DisplayLink(const DisplayLink& other) noexcept:
            displayLink{other.displayLink}
        {
            CVDisplayLinkRetain(displayLink);
        }

        DisplayLink& operator=(DisplayLink&& other) noexcept
        {
            if (&other == this) return *this;
            CVDisplayLinkRelease(displayLink);
            displayLink = other.displayLink;
            other.displayLink = nullptr;
            return *this;
        }

        DisplayLink& operator=(const DisplayLink& other) noexcept
        {
            if (&other == this) return *this;
            CVDisplayLinkRetain(other.displayLink);
            CVDisplayLinkRelease(displayLink);
            displayLink = other.displayLink;
            return *this;
        }

        void setCallback(const CVDisplayLinkOutputCallback callback, void* userInfo)
        {
            if (const auto result = CVDisplayLinkSetOutputCallback(displayLink, callback, userInfo); result != kCVReturnSuccess)
                throw std::system_error{result, errorCategory, "Failed to set output callback for the display link"};
        }

        void start()
        {
            if (displayLink)
                if (const auto result = CVDisplayLinkStart(displayLink); result != kCVReturnSuccess)
                    throw std::system_error{result, errorCategory, "Failed to start display link"};
        }

        void stop()
        {
            if (displayLink)
                if (const auto result = CVDisplayLinkStop(displayLink); result != kCVReturnSuccess)
                    throw std::system_error{result, errorCategory, "Failed to stop display link"};
        }

    private:
        CVDisplayLinkRef displayLink = nullptr;
    };
}

#endif // OUZEL_PLATFORM_COREVIDEO_DISPLAYLINK_HPP
