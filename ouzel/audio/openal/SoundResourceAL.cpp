// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENAL

#include "SoundResourceAL.h"
#include "AudioAL.h"
#include "audio/SoundData.h"
#include "audio/Stream.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace audio
    {
        SoundResourceAL::SoundResourceAL()
        {
        }

        SoundResourceAL::~SoundResourceAL()
        {
        }
    } // namespace audio
} // namespace ouzel

#endif
