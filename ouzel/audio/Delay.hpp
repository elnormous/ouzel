// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_DELAY_HPP
#define OUZEL_AUDIO_DELAY_HPP

#include "audio/SoundInput.hpp"
#include "audio/SoundOutput.hpp"

namespace ouzel
{
    namespace audio
    {
        class Delay final: public SoundInput, public SoundOutput
        {
        public:
            Delay();

            Delay(const Delay&) = delete;
            Delay& operator=(const Delay&) = delete;

            Delay(Delay&&) = delete;
            Delay& operator=(Delay&&) = delete;
            
            float getDelay() const { return delay; }
            void setDelay(float newDelay);

        private:
            uintptr_t nodeId = 0;
            float delay = 0.0F;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_DELAY_HPP
