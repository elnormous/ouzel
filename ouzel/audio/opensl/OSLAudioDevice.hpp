// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICESL_HPP
#define OUZEL_AUDIO_AUDIODEVICESL_HPP

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
        class OSLAudioDevice final: public AudioDevice
        {
        public:
            explicit OSLAudioDevice(uint32_t initBufferSize,
                                    uint32_t initSampleRate,
                                    uint16_t initChannels,
                                    const std::function<void(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)>& initDataGetter);
            ~OSLAudioDevice();

            void enqueue(SLAndroidSimpleBufferQueueItf bufferQueue);

            inline SLEngineItf getEngine() const { return engine; }
            inline SLObjectItf getOutputMix() const { return outputMixObject; }

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

#endif // OUZEL_AUDIO_AUDIODEVICESL_HPP
