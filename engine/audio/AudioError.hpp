// Ouzel by Elviss Strazdins

#ifndef OUZEL_STORAGE_AUDIOERROR_HPP
#define OUZEL_STORAGE_AUDIOERROR_HPP

#include <stdexcept>

namespace ouzel::audio
{
    class Error final: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
}

#endif // OUZEL_STORAGE_AUDIOERROR_HPP
