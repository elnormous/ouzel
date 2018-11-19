// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Gain.hpp"
#include "Node.hpp"

namespace ouzel
{
    namespace audio
    {
        class GainProcessor final: public Node
        {
        public:
            GainProcessor()
            {
            }
        };

        Gain::Gain(Audio& initAudio):
            audio(initAudio)
        {
        }

        void Gain::setGain(float newGain)
        {
            gain = newGain;
        }
    } // namespace audio
} // namespace ouzel
