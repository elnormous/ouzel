// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_SOUNDDATA_HPP
#define OUZEL_AUDIO_SOUNDDATA_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ouzel
{
    namespace audio
    {
        class Audio;

        class Sound
        {
            friend Audio;
        public:
            Sound(Audio& initAudio, uintptr_t initSourceDataId);
            virtual ~Sound();

            Sound(const Sound&) = delete;
            Sound& operator=(const Sound&) = delete;

            Sound(Sound&&) = delete;
            Sound& operator=(Sound&&) = delete;

            uintptr_t getSourceDataId() const { return sourceDataId; }

            inline uint16_t getChannels() const { return channels; }
            inline uint32_t getSampleRate() const { return sampleRate; }

        protected:
            Audio& audio;
            uintptr_t sourceDataId = 0;

            uint16_t channels = 0;
            uint32_t sampleRate = 0;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_SOUNDDATA_HPP
