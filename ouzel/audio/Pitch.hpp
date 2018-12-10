// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_PITCH_HPP
#define OUZEL_AUDIO_PITCH_HPP

#include "audio/Filter.hpp"

namespace ouzel
{
    namespace audio
    {
        class Pitch final: public Filter
        {
        public:
            Pitch(Audio& initAudio, float initPitch = 1.0F);
            ~Pitch();

            Pitch(const Pitch&) = delete;
            Pitch& operator=(const Pitch&) = delete;
            Pitch(Pitch&&) = delete;
            Pitch& operator=(Pitch&&) = delete;

            float getPitch() const { return pitch; }
            void setPitch(float newPitch);

        private:
            float pitch = 1.0f;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_PITCH_HPP
