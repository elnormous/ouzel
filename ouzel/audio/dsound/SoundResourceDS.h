// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_DIRECTSOUND

#include <dsound.h>

#include "audio/SoundResource.h"

namespace ouzel
{
    namespace audio
    {
        class SoundResourceDS: public SoundResource
        {
        public:
            SoundResourceDS();
            virtual ~SoundResourceDS();
        };
    } // namespace audio
} // namespace ouzel

#endif
