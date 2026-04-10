#ifndef FLUXION_AUDIO_WAVEPLAYER_HPP
#define FLUXION_AUDIO_WAVEPLAYER_HPP

#include "Source.hpp"
#include "Sound.hpp"

namespace fluxion::audio
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

#endif // FLUXION_AUDIO_WAVEPLAYER_HPP
