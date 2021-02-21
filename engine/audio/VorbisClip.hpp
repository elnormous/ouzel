// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_VORBISCLIP_HPP
#define OUZEL_AUDIO_VORBISCLIP_HPP

#include <cstdint>
#include <vector>
#include "Sound.hpp"

namespace ouzel::audio
{
    class VorbisData;

    class VorbisClip final: public Sound
    {
    public:
        VorbisClip(Audio& initAudio, const std::vector<std::byte>& initData);

    private:
        VorbisData* data;
    };
}

#endif // OUZEL_AUDIO_VORBISCLIP_HPP
