// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENSL

#include "SoundResourceSL.h"
#include "AudioSL.h"
#include "audio/SoundData.h"
#include "audio/Stream.h"
#include "core/Engine.h"

namespace ouzel
{
    namespace audio
    {
        SoundResourceSL::SoundResourceSL()
        {
        }

        SoundResourceSL::~SoundResourceSL()
        {
        }
    } // namespace audio
} // namespace ouzel

#endif
