#ifndef FLUXION_AUDIO_AUDIOERROR_HPP
#define FLUXION_AUDIO_AUDIOERROR_HPP

#include <stdexcept>

namespace fluxion::audio
{
    class Error final: public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };
}

#endif // FLUXION_AUDIO_AUDIOERROR_HPP
