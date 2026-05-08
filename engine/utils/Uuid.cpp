#include "Uuid.hpp"

#include <cctype>
#include <random>

#if defined(_WIN32)
#  pragma push_macro("WIN32_LEAN_AND_MEAN")
#  pragma push_macro("NOMINMAX")
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#  include <bcrypt.h>
#  pragma pop_macro("WIN32_LEAN_AND_MEAN")
#  pragma pop_macro("NOMINMAX")
#  pragma comment(lib, "bcrypt.lib")
#endif

namespace fluxion
{
    namespace
    {
        // Fill `bytes` with cryptographically secure random data when the
        // platform offers it; otherwise fall back to std::random_device,
        // which is implementation-defined but is the strongest portable
        // primitive C++17 gives us. The fallback path is documented as
        // best-effort and is used on platforms we have not specialised yet.
        void fillRandom(std::array<std::uint8_t, 16>& bytes) noexcept
        {
#if defined(_WIN32)
            const NTSTATUS status = BCryptGenRandom(nullptr,
                                                    bytes.data(),
                                                    static_cast<ULONG>(bytes.size()),
                                                    BCRYPT_USE_SYSTEM_PREFERRED_RNG);
            if (status == 0) return;  // STATUS_SUCCESS
            // Fall through to portable path on the unexpected failure case
            // rather than aborting; the resulting UUID will still be unique
            // with extremely high probability.
#endif
            std::random_device rd;
            std::uniform_int_distribution<std::uint32_t> dist{0, 0xFFu};
            for (auto& b : bytes) b = static_cast<std::uint8_t>(dist(rd));
        }

        constexpr int hexValue(char c) noexcept
        {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            return -1;
        }

        constexpr char hexChar(int v) noexcept
        {
            constexpr char digits[] = "0123456789abcdef";
            return digits[v & 0xF];
        }
    }

    Uuid Uuid::generate()
    {
        std::array<std::uint8_t, 16> bytes{};
        fillRandom(bytes);

        // RFC 4122: set the version (top nibble of byte 6) and variant
        // (top two bits of byte 8). Anything that validates a v4 UUID
        // checks these.
        bytes[6] = static_cast<std::uint8_t>((bytes[6] & 0x0F) | 0x40);
        bytes[8] = static_cast<std::uint8_t>((bytes[8] & 0x3F) | 0x80);

        std::uint64_t hi = 0, lo = 0;
        for (int i = 0; i < 8; ++i) hi = (hi << 8) | bytes[i];
        for (int i = 8; i < 16; ++i) lo = (lo << 8) | bytes[i];
        return Uuid{hi, lo};
    }

    std::optional<Uuid> Uuid::parse(std::string_view text) noexcept
    {
        // Strip optional surrounding braces.
        if (text.size() >= 2 && text.front() == '{' && text.back() == '}')
            text = text.substr(1, text.size() - 2);

        if (text.size() != 36) return std::nullopt;
        if (text[8] != '-' || text[13] != '-' ||
            text[18] != '-' || text[23] != '-')
            return std::nullopt;

        std::array<std::uint8_t, 16> bytes{};
        std::size_t outIndex = 0;
        for (std::size_t i = 0; i < text.size(); ++i)
        {
            if (text[i] == '-') continue;
            const int hi = hexValue(text[i]);
            if (hi < 0 || ++i >= text.size()) return std::nullopt;
            const int lo = hexValue(text[i]);
            if (lo < 0) return std::nullopt;
            if (outIndex >= bytes.size()) return std::nullopt;
            bytes[outIndex++] = static_cast<std::uint8_t>((hi << 4) | lo);
        }
        if (outIndex != bytes.size()) return std::nullopt;

        std::uint64_t hi = 0, lo = 0;
        for (int i = 0; i < 8; ++i) hi = (hi << 8) | bytes[static_cast<std::size_t>(i)];
        for (int i = 8; i < 16; ++i) lo = (lo << 8) | bytes[static_cast<std::size_t>(i)];
        return Uuid{hi, lo};
    }

    std::string Uuid::toString() const
    {
        std::array<std::uint8_t, 16> bytes{};
        for (int i = 0; i < 8; ++i)
            bytes[static_cast<std::size_t>(i)] =
                static_cast<std::uint8_t>((high_ >> (8 * (7 - i))) & 0xFF);
        for (int i = 0; i < 8; ++i)
            bytes[static_cast<std::size_t>(8 + i)] =
                static_cast<std::uint8_t>((low_ >> (8 * (7 - i))) & 0xFF);

        std::string out;
        out.resize(36);
        std::size_t pos = 0;
        for (std::size_t i = 0; i < bytes.size(); ++i)
        {
            if (i == 4 || i == 6 || i == 8 || i == 10) out[pos++] = '-';
            out[pos++] = hexChar((bytes[i] >> 4) & 0xF);
            out[pos++] = hexChar(bytes[i] & 0xF);
        }
        return out;
    }
}
