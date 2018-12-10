// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_SOUND_HPP
#define OUZEL_AUDIO_SOUND_HPP

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

        protected:
            Audio& audio;
            uintptr_t sourceDataId = 0;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_SOUND_HPP
