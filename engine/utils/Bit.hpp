#ifndef FLUXION_UTILS_BIT_HPP
#define FLUXION_UTILS_BIT_HPP

#include <cstring>
#include <type_traits>

namespace fluxion
{
    template <typename To, typename From>
    std::enable_if_t<
        (sizeof(To) == sizeof(From)) &&
        std::is_trivially_copyable_v<From> &&
        std::is_trivial_v<To> &&
        (std::is_copy_constructible_v<To> || std::is_move_constructible_v<To>),
        To
    >
    bitCast(const From& src) noexcept
    {
        To dst;
        std::memcpy(&dst, &src, sizeof(To));
        return dst;
    }
}

#endif // FLUXION_UTILS_BIT_HPP
