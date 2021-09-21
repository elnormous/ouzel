// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_SOURCE_HPP
#define OUZEL_AUDIO_SOURCE_HPP

#include <vector>
#include "Effect.hpp"

namespace ouzel::audio
{
    class Source
    {
    public:
        virtual ~Source() = default;

        virtual void play() {}
        virtual void stop([[maybe_unused]] bool shouldReset) {}
        virtual void getSamples(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate, std::vector<float>& samples) = 0;

        auto& getEffects() const noexcept { return effects; }

    private:
        std::vector<std::unique_ptr<Effect>> effects;
    };
}

#endif // OUZEL_AUDIO_SOURCE_HPP
