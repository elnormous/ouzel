#ifndef FLUXION_INPUT_DEVICEID_HPP
#define FLUXION_INPUT_DEVICEID_HPP

#include <typeindex> 

namespace fluxion::input
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
    template <> struct hash<fluxion::input::DeviceId>
    {
        [[nodiscard]] size_t operator()(const fluxion::input::DeviceId& deviceId) const noexcept
        {
            return hash<size_t>()(deviceId.value);
        }
    };
}

#endif // FLUXION_INPUT_DEVICEID_HPP
