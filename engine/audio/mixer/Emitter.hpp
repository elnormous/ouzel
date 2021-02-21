// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_MIXER_EMITTER_HPP
#define OUZEL_AUDIO_MIXER_EMITTER_HPP

#include <cstdint>
#include <vector>
#include "Source.hpp"

namespace ouzel::audio::mixer
{
    class Emitter: public Source
    {
    public:
        Emitter() noexcept = default;

        Emitter(const Emitter&) = delete;
        Emitter& operator=(const Emitter&) = delete;

        Emitter(Emitter&&) = delete;
        Emitter& operator=(Emitter&&) = delete;

        virtual void play() = 0;
        virtual void stop(bool shouldReset) = 0;

        virtual void getSamples(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate, std::vector<float>& samples) = 0;
    };
}

#endif // OUZEL_AUDIO_MIXER_EMITTER_HPP
