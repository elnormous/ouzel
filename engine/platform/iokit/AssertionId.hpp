// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_IOKIT_ASSERTIONID_HPP
#define OUZEL_PLATFORM_IOKIT_ASSERTIONID_HPP

#include <system_error>
#include <IOKit/pwr_mgt/IOPMLib.h>
#include "ErrorCategory.hpp"

namespace ouzel::platform::iokit
{
    class AssertionId final
    {
    public:
        AssertionId() noexcept = default;
        ~AssertionId() noexcept
        {
            if (assertionId) IOPMAssertionRelease(assertionId);
        }

        AssertionId(const AssertionId& other) noexcept:
            assertionId{other.assertionId}
        {
            if (assertionId) IOPMAssertionRetain(assertionId);
        }

        AssertionId& operator=(const AssertionId& other) noexcept
        {
            if (&other == this) return *this;

            if (assertionId) IOPMAssertionRelease(assertionId);
            assertionId = other.assertionId;
            if (assertionId) IOPMAssertionRetain(assertionId);

            return *this;
        }

        AssertionId(AssertionId&& other) noexcept:
            assertionId{other.assertionId}
        {
            other.assertionId = 0;
        }

        AssertionId& operator=(AssertionId&& other) noexcept
        {
            if (&other == this) return *this;

            if (assertionId) IOPMAssertionRelease(assertionId);
            assertionId = other.assertionId;
            other.assertionId = 0;

            return *this;
        }

        AssertionId(CFStringRef          assertionType,
                    IOPMAssertionLevel   assertionLevel,
                    CFStringRef          assertionName)
        {
            if (const auto error = IOPMAssertionCreateWithName(assertionType,
                                                               assertionLevel,
                                                               assertionName,
                                                               &assertionId); error != kIOReturnSuccess)
                throw std::system_error{error, errorCategory, "Failed to disable screen saver"};
        }

        operator bool() const noexcept { return assertionId != 0; }

        bool operator==(const AssertionId& other) const noexcept { return assertionId == other.assertionId; }
        bool operator!=(const AssertionId& other) const noexcept { return assertionId != other.assertionId; }

        void release()
        {
            if (assertionId)
                if (const auto error = IOPMAssertionRelease(assertionId); error != kIOReturnSuccess)
                    throw std::system_error{error, errorCategory, "Failed to enable screen saver"};
        }

    private:
        IOPMAssertionID assertionId = 0;
    };
}

#endif // OUZEL_PLATFORM_IOKIT_ASSERTIONID_HPP
