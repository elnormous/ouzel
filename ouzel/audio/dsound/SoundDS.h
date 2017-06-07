// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <dsound.h>

#include "audio/SoundResource.h"

namespace ouzel
{
    namespace audio
    {
        class SoundDS: public SoundResource
        {
        public:
            SoundDS();
            virtual ~SoundDS();

        protected:
            virtual bool update() override;
        };
    } // namespace audio
} // namespace ouzel
