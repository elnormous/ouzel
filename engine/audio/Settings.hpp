#ifndef FLUXION_AUDIO_SETTINGS_HPP
#define FLUXION_AUDIO_SETTINGS_HPP

#include <cstdint>
#include "SampleFormat.hpp"

namespace fluxion::audio
{
    struct Settings final
    {
        bool debugAudio = false;
        std::uint32_t bufferSize = 512;
        std::uint32_t sampleRate = 44100;
        std::uint32_t channels = 0;
        SampleFormat sampleFormat = SampleFormat::float32;
        std::string audioDevice;
    };
}

#endif // FLUXION_AUDIO_SETTINGS_HPP
