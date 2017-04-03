// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/SoundResource.h"

namespace ouzel
{
    namespace audio
    {
        class SoundEmpty: public SoundResource
        {
        public:
            SoundEmpty();

        protected:
            virtual bool update() override;
        };
    } // namespace audio
} // namespace ouzel
