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

        protected:
            AudioDS();
            virtual bool init() override;
        };
    } // namespace audio
} // namespace ouzel
