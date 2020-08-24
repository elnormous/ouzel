// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_WASAPIAUDIODEVICE_HPP
#define OUZEL_AUDIO_WASAPIAUDIODEVICE_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_WASAPI

#include <atomic>
#include <thread>
#include <Audioclient.h>
#include <mmdeviceapi.h>
#include "../AudioDevice.hpp"
#include "WASAPIPointer.hpp"
#include "../../thread/Thread.hpp"

namespace ouzel::audio::wasapi
{
    class AudioDevice final: public audio::AudioDevice
    {
    public:
        AudioDevice(const Settings& settings,
                    const std::function<void(std::uint32_t frames,
                                                std::uint32_t channels,
                                                std::uint32_t sampleRate,
                                                std::vector<float>& samples)>& initDataGetter);
        ~AudioDevice() override;

        void start() final;
        void stop() final;

    private:
        void run();

        Pointer<IMMDeviceEnumerator> enumerator;
        Pointer<IMMDevice> device;
        Pointer<IMMNotificationClient> notificationClient;
        Pointer<IAudioClient> audioClient;
        Pointer<IAudioRenderClient> renderClient;
        HANDLE notifyEvent = nullptr;

        UINT32 bufferFrameCount;
        std::uint32_t sampleSize = 0;
        bool started = false;
        std::vector<std::uint8_t> data;

        std::atomic_bool running{false};
        thread::Thread audioThread;
    };
}
#endif

#endif // OUZEL_AUDIO_WASAPIAUDIODEVICE_HPP
