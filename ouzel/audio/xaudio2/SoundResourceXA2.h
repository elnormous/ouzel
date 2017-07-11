// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_XAUDIO2

#include <cstdint>
#include <vector>
#include <xaudio2.h>

#include "audio/SoundResource.h"

namespace ouzel
{
    namespace audio
    {
        class SoundResourceXA2: public SoundResource
        {
        public:
            SoundResourceXA2();
            virtual ~SoundResourceXA2();
        };
    } // namespace audio
} // namespace ouzel

#endif
