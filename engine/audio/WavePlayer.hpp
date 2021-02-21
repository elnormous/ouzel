// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_WAVEPLAYER_HPP
#define OUZEL_AUDIO_WAVEPLAYER_HPP

#include "Source.hpp"
#include "Sound.hpp"

namespace ouzel::audio
{
    class VorbisData;

    class WavePlayer final: public Source
    {
    public:
        explicit WavePlayer(const Sound*)
        {
        }

        void getSamples(std::uint32_t, std::uint32_t, std::uint32_t, std::vector<float>&) override
        {
        }

    private:
    };
}

#endif // OUZEL_AUDIO_WAVEPLAYER_HPP
