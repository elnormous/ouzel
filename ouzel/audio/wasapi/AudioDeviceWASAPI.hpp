// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICEWASAPI_HPP
#define OUZEL_AUDIO_AUDIODEVICEWASAPI_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_WASAPI

#include <Audioclient.h>
#include <mmdeviceapi.h>
#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioDeviceWASAPI final: public AudioDevice
        {
        public:
            explicit AudioDeviceWASAPI(Mixer& initMixer);
            ~AudioDeviceWASAPI();

        private:
            IMMDeviceEnumerator* enumerator = nullptr;
            IMMDevice* device = nullptr;
            IMMNotificationClient* notificationClient = nullptr;
            IAudioClient* audioClient = nullptr;
            IAudioRenderClient* renderClient = nullptr;

            uint32_t sampleSize = 0;
        };
    } // namespace audio
} // namespace ouzel

#endif

#endif // OUZEL_AUDIO_AUDIODEVICEWASAPI_HPP
