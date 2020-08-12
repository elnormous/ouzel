// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_OALERRORCATEGORY_HPP
#define OUZEL_AUDIO_OALERRORCATEGORY_HPP

#include <stdexcept>

namespace ouzel::audio::openal
{
    class OpenALErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept final
        {
            return "OpenAL";
        }

        std::string message(int condition) const final
        {
            switch (condition)
            {
                case AL_INVALID_NAME: return "AL_INVALID_NAME";
                case AL_INVALID_ENUM: return "AL_INVALID_ENUM";
                case AL_INVALID_VALUE: return "AL_INVALID_VALUE";
                case AL_INVALID_OPERATION: return "AL_INVALID_OPERATION";
                case AL_OUT_OF_MEMORY: return "AL_OUT_OF_MEMORY";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };
}

#endif // OUZEL_AUDIO_OALERRORCATEGORY_HPP
