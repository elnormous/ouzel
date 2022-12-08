// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_DEVICEID_HPP
#define OUZEL_INPUT_DEVICEID_HPP

#include <typeindex> 

namespace ouzel::input
{
    struct DeviceId final
    {
        std::size_t value = 0;

        [[nodiscard]] constexpr bool operator==(DeviceId other) const noexcept
        {
            return value == other.value;
        }
    };
}

namespace std
{
    template <> struct hash<ouzel::input::DeviceId>
    {
        [[nodiscard]] size_t operator()(const ouzel::input::DeviceId& deviceId) const noexcept
        {
            return hash<size_t>()(deviceId.value);
        }
    };
}

#endif // OUZEL_INPUT_DEVICEID_HPP
