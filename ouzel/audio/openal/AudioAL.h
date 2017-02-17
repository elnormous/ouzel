// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include <OpenAl/al.h>
#include <OpenAl/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include "audio/Audio.h"

namespace ouzel
{
    namespace audio
    {
        class AudioAL: public Audio
        {
            friend Engine;
        public:
            bool checkALCError(bool logError = true);
            static bool checkOpenALError(bool logError = true);

            virtual ~AudioAL();

            virtual void setListenerPosition(const Vector3& newPosition) override;
            virtual void setListenerOrientation(const Vector3& newOrientation) override;

            virtual SoundPtr createSound() override;

            ALCdevice* getDevice() const { return device; }
            ALCcontext* getContext() const { return context; }

        protected:
            AudioAL();
            virtual bool init() override;

            ALCdevice* device = nullptr;
            ALCcontext* context = nullptr;
        };
    } // namespace audio
} // namespace ouzel
