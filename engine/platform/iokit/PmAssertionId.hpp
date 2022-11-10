// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_IOKIT_PMASSERTIONID_HPP
#define OUZEL_PLATFORM_IOKIT_PMASSERTIONID_HPP

#include <system_error>
#include <IOKit/pwr_mgt/IOPMLib.h>
#include "ErrorCategory.hpp"

namespace ouzel::platform::iokit
{
    class PmAssertionId final
    {
    public:
        PmAssertionId() noexcept = default;
        ~PmAssertionId() noexcept
        {
            if (assertionId) IOPMAssertionRelease(assertionId);
        }

        PmAssertionId(const PmAssertionId& other) noexcept:
            assertionId{other.assertionId}
        {
            if (assertionId) IOPMAssertionRetain(assertionId);
        }

        PmAssertionId& operator=(const PmAssertionId& other) noexcept
        {
            if (&other == this) return *this;

            if (other.assertionId) IOPMAssertionRetain(other.assertionId);
            if (assertionId) IOPMAssertionRelease(assertionId);
            assertionId = other.assertionId;

            return *this;
        }

        PmAssertionId(PmAssertionId&& other) noexcept:
            assertionId{other.assertionId}
        {
            other.assertionId = 0;
        }

        PmAssertionId& operator=(PmAssertionId&& other) noexcept
        {
            if (&other == this) return *this;

            if (assertionId) IOPMAssertionRelease(assertionId);
            assertionId = other.assertionId;
            other.assertionId = 0;

            return *this;
        }

        PmAssertionId(CFStringRef          assertionType,
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

        bool operator==(const PmAssertionId& other) const noexcept { return assertionId == other.assertionId; }
        bool operator!=(const PmAssertionId& other) const noexcept { return assertionId != other.assertionId; }

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

#endif // OUZEL_PLATFORM_IOKIT_PMASSERTIONID_HPP
