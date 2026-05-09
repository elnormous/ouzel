#ifndef FLUXION_AUDIO_VORBISCLIP_HPP
#define FLUXION_AUDIO_VORBISCLIP_HPP

#include <cstdint>
#include <vector>
#include "Sound.hpp"

namespace fluxion::audio
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

#endif // FLUXION_AUDIO_VORBISCLIP_HPP
