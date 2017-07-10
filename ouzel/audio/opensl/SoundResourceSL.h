// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENSL

#include <vector>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

#include "audio/SoundResource.h"

namespace ouzel
{
    namespace audio
    {
        class SoundResourceSL: public SoundResource
        {
        public:
            SoundResourceSL();
            virtual ~SoundResourceSL();
        };
    } // namespace audio
} // namespace ouzel

#endif
