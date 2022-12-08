// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_IOKIT_HIDMANAGER_HPP
#define OUZEL_PLATFORM_IOKIT_HIDMANAGER_HPP

#include <system_error>
#include <IOKit/hid/IOHIDManager.h>
#include "ErrorCategory.hpp"

namespace ouzel::platform::iokit
{
    class HidManager final
    {
    public:
        explicit HidManager(const IOOptionBits options = kIOHIDOptionsTypeNone) noexcept:
            hidManager{IOHIDManagerCreate(kCFAllocatorDefault, options)}
        {
        }

        ~HidManager()
        {
            if (hidManager) CFRelease(hidManager);
        }

        HidManager(const HidManager& other) noexcept: hidManager{other.hidManager}
        {
            if (hidManager) CFRetain(hidManager);
        }

        HidManager& operator=(const HidManager& other) noexcept
        {
            if (&other == this) return *this;
            if (other.hidManager) CFRetain(other.hidManager);
            if (hidManager) CFRelease(hidManager);
            hidManager = other.hidManager;
            return *this;
        }

        HidManager(HidManager&& other) noexcept: hidManager{other.hidManager}
        {
            other.hidManager = nullptr;
        }

        HidManager& operator=(HidManager&& other) noexcept
        {
            if (&other == this) return *this;
            if (hidManager) CFRelease(hidManager);
            hidManager = other.hidManager;
            other.hidManager = nullptr;
            return *this;
        }

        [[nodiscard]] operator IOHIDManagerRef() const noexcept
        {
            return hidManager;
        }

        [[nodiscard]] bool operator==(const HidManager& other) const noexcept
        {
            return hidManager == other.hidManager;
        }

        [[nodiscard]] bool operator!=(const HidManager& other) const noexcept
        {
            return hidManager != other.hidManager;
        }

        [[nodiscard]] bool operator==(std::nullptr_t) const noexcept
        {
            return hidManager == nullptr;
        }

        [[nodiscard]] bool operator!=(std::nullptr_t) const noexcept
        {
            return hidManager != nullptr;
        }

        void open(const IOOptionBits options = kIOHIDOptionsTypeNone)
        {
            if (const auto result = IOHIDManagerOpen(hidManager, options); result != kIOReturnSuccess)
                throw std::system_error{result, platform::iokit::errorCategory, "Failed to open HID manager"};
        }

        void close(const IOOptionBits options = kIOHIDOptionsTypeNone)
        {
            if (const auto result = IOHIDManagerClose(hidManager, options); result != kIOReturnSuccess)
                throw std::system_error{result, platform::iokit::errorCategory, "Failed to close HID manager"};
        }

    private:
        IOHIDManagerRef hidManager = nullptr;
    };
}

#endif // OUZEL_PLATFORM_IOKIT_HIDMANAGER_HPP
