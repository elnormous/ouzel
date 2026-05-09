#ifndef FLUXION_AUDIO_MIXER_DATA_HPP
#define FLUXION_AUDIO_MIXER_DATA_HPP

#include <memory>
#include "Object.hpp"

namespace fluxion::audio::mixer
{
    class Stream;

    class Data: public Object
    {
    public:
        Data() noexcept = default;
        Data(std::uint32_t initChannels, std::uint32_t initSampleRate) noexcept:
            channels{initChannels}, sampleRate{initSampleRate}
        {
        }

        virtual std::unique_ptr<Stream> createStream() = 0;

        auto getChannels() const noexcept { return channels; }
        auto getSampleRate() const noexcept { return sampleRate; }

    protected:
        std::uint32_t channels = 0;
        std::uint32_t sampleRate = 0;
    };
}

#endif // FLUXION_AUDIO_MIXER_DATA_HPP
