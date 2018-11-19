// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_PITCH_HPP
#define OUZEL_AUDIO_PITCH_HPP

#include "audio/Source.hpp"
#include "audio/Destination.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;

        class Pitch final: public Source, public Destination
        {
        public:
            Pitch(Audio& initAudio);

            Pitch(const Pitch&) = delete;
            Pitch& operator=(const Pitch&) = delete;

            Pitch(Pitch&&) = delete;
            Pitch& operator=(Pitch&&) = delete;
            
            float getPitch() const { return pitch; }
            void setPitch(float newPitch);

        private:
            Audio& audio;
            uintptr_t nodeId = 0;
            float pitch = 1.0f;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_PITCH_HPP
