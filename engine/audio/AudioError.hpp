// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_AUDIOERROR_HPP
#define OUZEL_AUDIO_AUDIOERROR_HPP

#include <stdexcept>

namespace ouzel::audio
{
    class Error final: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
}

#endif // OUZEL_AUDIO_AUDIOERROR_HPP
