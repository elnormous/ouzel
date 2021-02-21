// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_PCMCLIP_HPP
#define OUZEL_AUDIO_PCMCLIP_HPP

#include <cstdint>
#include <vector>
#include "Sound.hpp"

namespace ouzel::audio
{
    class PcmData;

    class PcmClip final: public Sound
    {
    public:
        PcmClip(Audio& initAudio, std::uint32_t channels, std::uint32_t sampleRate,
                const std::vector<float>& samples);

    private:
        PcmData* data;
    };
}

#endif // OUZEL_AUDIO_PCMCLIP_HPP
