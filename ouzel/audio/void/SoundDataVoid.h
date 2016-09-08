// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/SoundData.h"

namespace ouzel
{
    namespace audio
    {
        class AudioVoid;

        class SoundDataVoid: public SoundData
        {
            friend AudioVoid;
        public:
            virtual ~SoundDataVoid();

        protected:
            SoundDataVoid();
        };
    } // namespace audio
} // namespace ouzel
