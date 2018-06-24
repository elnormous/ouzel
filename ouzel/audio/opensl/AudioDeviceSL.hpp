// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENSL

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioDeviceSL: public AudioDevice
        {
            friend Audio;
        public:
            virtual ~AudioDeviceSL();

            void enqueue(SLAndroidSimpleBufferQueueItf bufferQueue);

            SLEngineItf getEngine() const { return engine; }
            SLObjectItf getOutputMix() const { return outputMixObject; }

        protected:
            AudioDeviceSL();
            virtual void init(bool debugAudio) override;

        private:
            SLObjectItf engineObject = nullptr;
            SLEngineItf engine = nullptr;
            SLObjectItf outputMixObject = nullptr;

            SLObjectItf playerObject = nullptr;
            SLPlayItf player = nullptr;
            SLAndroidSimpleBufferQueueItf bufferQueue = nullptr;
            SLVolumeItf playerVolume = nullptr;

            std::vector<uint8_t> data;
        };
    } // namespace audio
} // namespace ouzel

#endif
