// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/openal/AudioAL.h"

namespace ouzel
{
    namespace audio
    {
        class AudioALApple: public AudioAL
        {
        public:
            virtual bool init() override;
        };
    }
}
