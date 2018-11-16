// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_GAIN_HPP
#define OUZEL_AUDIO_GAIN_HPP

namespace ouzel
{
    namespace audio
    {
        class Gain final
        {
        public:
            float getGain() const { return gain; }
            void setGain(float newGain);

        private:
            float gain = 0.0f;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_GAIN_HPP
