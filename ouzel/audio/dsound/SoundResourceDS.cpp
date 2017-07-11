// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_DIRECTSOUND

#include "SoundResourceDS.h"
#include "AudioDS.h"
#include "audio/SoundData.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace audio
    {
        SoundResourceDS::SoundResourceDS()
        {
        }

        SoundResourceDS::~SoundResourceDS()
        {
        }
    } // namespace audio
} // namespace ouzel

#endif
