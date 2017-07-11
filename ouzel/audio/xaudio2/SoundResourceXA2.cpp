// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_XAUDIO2

#include "SoundResourceXA2.h"
#include "AudioXA2.h"
#include "audio/SoundData.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace audio
    {
        SoundResourceXA2::SoundResourceXA2()
        {
        }

        SoundResourceXA2::~SoundResourceXA2()
        {
        }
    } // namespace audio
} // namespace ouzel

#endif
