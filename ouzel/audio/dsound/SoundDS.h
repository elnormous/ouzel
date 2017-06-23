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
        class SoundDS: public SoundResource
        {
        public:
            SoundDS();
            virtual ~SoundDS();

        protected:
            virtual bool update() override;

            IDirectSoundBuffer8* buffer = nullptr;
            IDirectSound3DBuffer8* buffer3D = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif
