// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENAL

#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
    #include <OpenAl/al.h>
    #include <OpenAl/alc.h>
#else
    #include <AL/al.h>
    #include <AL/alc.h>
#endif
#undef OPENAL

#include "audio/SoundResource.h"

namespace ouzel
{
    namespace audio
    {
        class SoundResourceAL: public SoundResource
        {
        public:
            SoundResourceAL();
            virtual ~SoundResourceAL();
        };
    } // namespace audio
} // namespace ouzel

#endif
