// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Types.h"

namespace ouzel
{
    namespace audio
    {
        class Sound
        {
            friend Audio;
        public:
            virtual ~Sound();

            virtual bool init(const SoundDataPtr& newSoundData);

        protected:
            Sound();

            SoundDataPtr soundData;
        };
    } // namespace audio
} // namespace ouzel
