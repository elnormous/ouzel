// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_PITCH_HPP
#define OUZEL_AUDIO_PITCH_HPP

namespace ouzel
{
    namespace audio
    {
        class Pitch final
        {
        public:
            float getPitch() const { return pitch; }
            void setPitch(float newPitch);

        private:
            float pitch = 1.0f;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_PITCH_HPP
