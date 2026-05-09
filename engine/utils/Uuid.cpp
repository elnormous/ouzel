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
#elif defined(__APPLE__)
// arc4random_buf lives in libSystem on every Apple platform we target
// (macOS 10.7+, iOS 4.3+, tvOS 9+, watchOS 2+). It is documented as a
// cryptographically secure source that never blocks and never fails.
#  include <stdlib.h>
#elif defined(__EMSCRIPTEN__)
#  include <emscripten.h>
#elif defined(__linux__) || defined(__ANDROID__)
// Try the modern getrandom() syscall first; fall back to /dev/urandom on
// older kernels (Linux pre-3.17, Android pre-API-28). The fallback path
// is also what the libstdc++ std::random_device picks, so the worst-
// case quality matches the standard library's.
#  include <fcntl.h>
#  include <unistd.h>
#  if defined(__linux__) && !defined(__ANDROID__)
#    include <sys/random.h>  // getrandom()
#  endif
#endif

namespace fluxion
{
    namespace
    {
        // Fill `bytes` with cryptographically secure random data. The
        // primary platform paths use the kernel's CSPRNG directly so we
        // are not at the mercy of std::random_device's implementation
        // quality (which is well-defined on libstdc++/libc++ but has been
        // weak on some MinGW builds historically). The std::random_device
        // tail is reached only on platforms we have not specialised; it
        // is also the well-known portable C++17 fallback.
        void fillRandom(std::array<std::uint8_t, 16>& bytes) noexcept
        {
#if defined(_WIN32)
            // BCryptGenRandom is the documented CSPRNG of Windows Vista+.
            // BCRYPT_USE_SYSTEM_PREFERRED_RNG removes the need to open a
            // provider handle and is itself thread-safe.
            const NTSTATUS status = BCryptGenRandom(nullptr,
                                                    bytes.data(),
                                                    static_cast<ULONG>(bytes.size()),
                                                    BCRYPT_USE_SYSTEM_PREFERRED_RNG);
            if (status == 0) return;  // STATUS_SUCCESS
#elif defined(__APPLE__)
            // arc4random_buf is documented as cryptographically secure on
            // every modern Apple OS (macOS 10.7+, iOS 4.3+, tvOS 9+).
            // It cannot fail.
            arc4random_buf(bytes.data(), bytes.size());
            return;
#elif defined(__EMSCRIPTEN__)
            // Browser path: use the WebCrypto getRandomValues API. The
            // EM_ASM block writes 16 bytes into the wasm heap at the
            // address of `bytes.data()`. crypto.getRandomValues throws if
            // count > 65536 (we use 16) and returns a Uint8Array.
            EM_ASM({
                const heap = HEAPU8.subarray($0, $0 + $1);
                crypto.getRandomValues(heap);
            }, bytes.data(), bytes.size());
            return;
#elif defined(__linux__) && !defined(__ANDROID__)
            // getrandom() is the kernel's CSPRNG entry on Linux 3.17+.
            // It blocks at boot until the entropy pool is initialised
            // (which is the right thing to do for a UUID generator).
            ssize_t got = 0;
            while (got < static_cast<ssize_t>(bytes.size()))
            {
                const ssize_t n = getrandom(bytes.data() + got,
                                            bytes.size() - static_cast<std::size_t>(got),
                                            0);
                if (n <= 0) break;  // EINTR or fatal; try /dev/urandom below
                got += n;
            }
            if (got == static_cast<ssize_t>(bytes.size())) return;
            // Fall through to /dev/urandom if getrandom is unavailable.
            const int fd = ::open("/dev/urandom", O_RDONLY | O_CLOEXEC);
            if (fd >= 0)
            {
                ssize_t total = 0;
                while (total < static_cast<ssize_t>(bytes.size()))
                {
                    const ssize_t n = ::read(fd, bytes.data() + total,
                                             bytes.size() - static_cast<std::size_t>(total));
                    if (n <= 0) break;
                    total += n;
                }
                ::close(fd);
                if (total == static_cast<ssize_t>(bytes.size())) return;
            }
#elif defined(__ANDROID__)
            // Android NDK has /dev/urandom on every supported API level.
            const int fd = ::open("/dev/urandom", O_RDONLY | O_CLOEXEC);
            if (fd >= 0)
            {
                ssize_t total = 0;
                while (total < static_cast<ssize_t>(bytes.size()))
                {
                    const ssize_t n = ::read(fd, bytes.data() + total,
                                             bytes.size() - static_cast<std::size_t>(total));
                    if (n <= 0) break;
                    total += n;
                }
                ::close(fd);
                if (total == static_cast<ssize_t>(bytes.size())) return;
            }
#endif
            // Portable C++17 fallback. On every standard library we
            // currently target this is itself a CSPRNG (libstdc++ uses
            // /dev/urandom, libc++ uses arc4random/getrandom, MSVC uses
            // RtlGenRandom). Reaching this branch on a platform we have
            // specialised above means the primary path failed.
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
