// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

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

            virtual void setListenerPosition(const Vector3& newPosition);
            virtual void setListenerRotation(const Quaternion& newRotation);

            virtual SoundResource* createSound() override;

            IDirectSound8* getDirectSound() const { return directSound; }

        protected:
            AudioDS();
            virtual bool init() override;

            IDirectSound8* directSound = nullptr;
        };
    } // namespace audio
} // namespace ouzel
