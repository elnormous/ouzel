#ifndef FLUXION_AUDIO_MIXER_MIXERERROR_HPP
#define FLUXION_AUDIO_MIXER_MIXERERROR_HPP

#include <stdexcept>

namespace fluxion::audio::mixer
{
    class Error final: public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };
}

#endif // FLUXION_AUDIO_MIXER_MIXERERROR_HPP
