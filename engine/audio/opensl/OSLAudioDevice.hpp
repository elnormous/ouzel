// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_OSLAUDIODEVICE_HPP
#define OUZEL_AUDIO_OSLAUDIODEVICE_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENSL

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

#include "../AudioDevice.hpp"
#include "OSLPointer.hpp"

namespace ouzel::audio::opensl
{
    class AudioDevice final: public audio::AudioDevice
    {
    public:
        AudioDevice(const Settings& settings,
                    const std::function<void(std::uint32_t frames,
                                                std::uint32_t channels,
                                                std::uint32_t sampleRate,
                                                std::vector<float>& samples)>& initDataGetter);

        void start() final;
        void stop() final;

        void enqueue(SLAndroidSimpleBufferQueueItf bufferQueue);

        auto getEngine() const noexcept { return engine; }

    private:
        Pointer<SLObjectItf> engineObject;
        SLEngineItf engine = nullptr;
        Pointer<SLObjectItf> outputMixObject;

        Pointer<SLObjectItf> playerObject;
        SLPlayItf player = nullptr;
        SLAndroidSimpleBufferQueueItf bufferQueue = nullptr;
        SLVolumeItf playerVolume = nullptr;

        std::vector<std::uint8_t> data;
    };
}
#endif

#endif // OUZEL_AUDIO_OSLAUDIODEVICE_HPP
