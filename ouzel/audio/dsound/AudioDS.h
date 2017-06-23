// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_DIRECTSOUND

#include <dsound.h>
#include "audio/Audio.h"

namespace ouzel
{
    namespace audio
    {
        class AudioDS: public Audio
        {
            friend Engine;
        public:
            virtual ~AudioDS();

            virtual bool update() override;

            virtual SoundResource* createSound() override;

            IDirectSound8* getDirectSound() const { return directSound; }

        protected:
            AudioDS();
            virtual bool init() override;

            IDirectSound8* directSound = nullptr;
            
            IDirectSoundBuffer* buffer = nullptr;
            IDirectSound3DListener8* listener3D = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif
