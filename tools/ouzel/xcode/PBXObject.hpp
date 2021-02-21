// Ouzel by Elviss Strazdins

#ifndef OUZEL_XCODE_PBXOBJECT_HPP
#define OUZEL_XCODE_PBXOBJECT_HPP

#include <array>
#include <chrono>
#include <random>
#include <string>
#include "formats/Plist.hpp"

namespace ouzel::xcode
{
    using Id = std::array<std::uint8_t, 12>;

    inline std::string toString(const Id& i)
    {
        std::string result;
        result.reserve(2 * i.size());
        for (const auto b : i)
        {
            constexpr char digits[] = "0123456789ABCDEF";
            result.push_back(digits[(b >> 4) & 0x0F]);
            result.push_back(digits[(b >> 0) & 0x0F]);
        }
        return result;
    }

    class PBXObject
    {
    public:
        PBXObject() = default;
        PBXObject(const Id& initId): id(initId) {}
        virtual ~PBXObject() = default;

        virtual std::string getIsa() const { return "PBXObject"; }
        const Id& getId() const noexcept { return id; }

        virtual plist::Value encode() const
        {
            return plist::Value::Dictionary{
                {"isa", getIsa()}
            };
        }

    private:
        static Id generateId()
        {
            static std::random_device randomDevice;
            static std::mt19937 randomEngine(randomDevice());
            static std::uint32_t sequence = 0;
            const auto s = sequence++;
            if (sequence >= 0x00FFFFFF) sequence = 0;
            const auto now = std::chrono::system_clock::now();
            const auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
            const std::uint32_t r = randomEngine();

            return {
                0x10, // to avoid collisions with Ouzel targets
                static_cast<std::uint8_t>((s >> 16) & 0xFF),
                static_cast<std::uint8_t>((s >> 8) & 0xFF),
                static_cast<std::uint8_t>((s >> 0) & 0xFF),

                static_cast<std::uint8_t>((timestamp >> 24) & 0xFF),
                static_cast<std::uint8_t>((timestamp >> 16) & 0xFF),
                static_cast<std::uint8_t>((timestamp >> 8) & 0xFF),
                static_cast<std::uint8_t>((timestamp >> 0) & 0xFF),

                static_cast<std::uint8_t>((r >> 24) & 0xFF),
                static_cast<std::uint8_t>((r >> 16) & 0xFF),
                static_cast<std::uint8_t>((r >> 8) & 0xFF),
                static_cast<std::uint8_t>((r >> 0) & 0xFF)
            };
        }

        Id id = generateId();
    };
}

#endif // OUZEL_XCODE_PBXOBJECT_HPP
