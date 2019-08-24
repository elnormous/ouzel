// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Effect.hpp"
#include "Audio.hpp"
#include "Submix.hpp"

namespace ouzel
{
    namespace audio
    {
        Effect::Effect(Audio& initAudio):
            audio(initAudio)
        {
        }
        void Effect::setEnabled(bool newEnabled)
        {
            enabled = newEnabled;
        }
    } // namespace audio
} // namespace ouzel
