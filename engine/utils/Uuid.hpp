#ifndef FLUXION_UTILS_UUID_HPP
#define FLUXION_UTILS_UUID_HPP

#include <array>
#include <cstdint>
#include <cstring>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

namespace fluxion
{
    // 128-bit RFC 4122 UUID. Used as a stable, scene-file-portable identifier
    // for Actors, Assets and similar editor entities.
    //
    // The default-constructed value is the nil UUID (all zero bits) and
    // compares equal to itself; it is the canonical "unassigned" sentinel.
    //
    // generate() produces a v4 (random) UUID using the most secure source
    // available on the platform: BCryptGenRandom on Windows, /dev/urandom
    // on POSIX, falling back to std::random_device. The two RFC 4122
    // version/variant bit patterns are imposed afterwards so the result
    // round-trips through any v4-validating peer.
    class Uuid final
    {
    public:
        constexpr Uuid() noexcept = default;

        constexpr Uuid(std::uint64_t high, std::uint64_t low) noexcept:
            high_{high}, low_{low} {}

        // Equivalent to {0, 0}; explicit name for code clarity.
        [[nodiscard]] constexpr bool isNull() const noexcept
        {
            return high_ == 0 && low_ == 0;
        }

        // RFC 4122 v4. Cryptographically random when the platform supports it.
        [[nodiscard]] static Uuid generate();

        // Accept the canonical 8-4-4-4-12 form, case-insensitive, with or
        // without surrounding braces. Returns std::nullopt on any error
        // (length, hex, missing dashes, etc.) — never throws.
        [[nodiscard]] static std::optional<Uuid> parse(std::string_view text) noexcept;

        // Lowercase canonical form: 36 chars with dashes (no braces).
        [[nodiscard]] std::string toString() const;

        [[nodiscard]] constexpr std::uint64_t high() const noexcept { return high_; }
        [[nodiscard]] constexpr std::uint64_t low() const noexcept { return low_; }

        [[nodiscard]] constexpr bool operator==(const Uuid& other) const noexcept
        {
            return high_ == other.high_ && low_ == other.low_;
        }
        [[nodiscard]] constexpr bool operator!=(const Uuid& other) const noexcept
        {
            return !(*this == other);
        }
        [[nodiscard]] constexpr bool operator<(const Uuid& other) const noexcept
        {
            return high_ < other.high_ || (high_ == other.high_ && low_ < other.low_);
        }

    private:
        std::uint64_t high_ = 0;
        std::uint64_t low_ = 0;
    };
}

namespace std
{
    template <>
    struct hash<::fluxion::Uuid>
    {
        std::size_t operator()(const ::fluxion::Uuid& id) const noexcept
        {
            // Mix high/low with a non-trivial constant so collisions on
            // either half don't directly become hash collisions; this is
            // the same boost::hash_combine constant.
            const std::size_t h1 = std::hash<std::uint64_t>{}(id.high());
            const std::size_t h2 = std::hash<std::uint64_t>{}(id.low());
            return h1 ^ (h2 + 0x9E3779B97F4A7C15ULL + (h1 << 6) + (h1 >> 2));
        }
    };
}

#endif // FLUXION_UTILS_UUID_HPP
