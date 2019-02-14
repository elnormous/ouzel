// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICEWASAPI_HPP
#define OUZEL_AUDIO_AUDIODEVICEWASAPI_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_WASAPI

#include <atomic>
#include <thread>
#include <Audioclient.h>
#include <mmdeviceapi.h>
#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class WASAPIAudioDevice final: public AudioDevice
        {
        public:
            explicit WASAPIAudioDevice(const std::function<void(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)>& initDataGetter);
            ~WASAPIAudioDevice();

        private:
            void run();

            IMMDeviceEnumerator* enumerator = nullptr;
            IMMDevice* device = nullptr;
            IMMNotificationClient* notificationClient = nullptr;
            IAudioClient* audioClient = nullptr;
            IAudioRenderClient* renderClient = nullptr;
            HANDLE notifyEvent = nullptr;

            UINT32 bufferFrameCount;
            uint32_t sampleSize = 0;
            bool started = false;
            std::vector<uint8_t> data;

            std::atomic_bool running{false};
            std::thread audioThread;
        };
    } // namespace audio
} // namespace ouzel

#endif

#endif // OUZEL_AUDIO_AUDIODEVICEWASAPI_HPP
