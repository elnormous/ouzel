// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_SOURCE_DATA_HPP
#define OUZEL_AUDIO_MIXER_SOURCE_DATA_HPP

#include <memory>
#include "audio/mixer/Object.hpp"

namespace ouzel
{
    namespace audio
    {
        class Source;

        class SourceData: public Object
        {
        public:
            virtual std::unique_ptr<Source> createSource() = 0;

            uint16_t getChannels() const { return channels; }
            uint32_t getSampleRate() const { return sampleRate; }

        protected:
            uint16_t channels = 0;
            uint32_t sampleRate = 0;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_SOURCE_DATA_HPP
