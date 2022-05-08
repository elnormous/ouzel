// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_MIXER_MIXERERROR_HPP
#define OUZEL_AUDIO_MIXER_MIXERERROR_HPP

#include <stdexcept>

namespace ouzel::audio::mixer
{
    class Error final: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
}

#endif // OUZEL_AUDIO_MIXER_MIXERERROR_HPP
