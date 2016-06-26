// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <OpenAl/al.h>
#include <OpenAl/alc.h>

#include "audio/Audio.h"

namespace ouzel
{
    namespace audio
    {
        class AudioAL: public Audio
        {
            friend Engine;
        public:
            static bool checkOpenALErrors(bool logError = true);
            
            virtual ~AudioAL();
            virtual void free() override;

            virtual bool init() override;

            virtual SoundDataPtr createSoundData() override;
            virtual SoundPtr createSound() override;

            ALCdevice* getDevice() const { return device; }
            ALCcontext* getContext() const { return context; }

        protected:
            AudioAL();

            ALCdevice* device = nullptr;
            ALCcontext* context = nullptr;
        };
    } // namespace audio
} // namespace ouzel
