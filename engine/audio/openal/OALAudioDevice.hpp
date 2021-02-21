// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_OALAUDIODEVICE_HPP
#define OUZEL_AUDIO_OALAUDIODEVICE_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENAL

#include <array>
#include <atomic>
#include <thread>

#if defined(__APPLE__)
#  include <OpenAl/al.h>
#  include <OpenAl/alc.h>
#else
#  include <AL/al.h>
#  include <AL/alc.h>
#endif

#include "../AudioDevice.hpp"
#include "../../thread/Thread.hpp"

namespace ouzel::audio::openal
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
        void process();

        auto getDevice() const noexcept { return device; }
        auto getContext() const noexcept { return context; }

    private:
        void run();

        ALCdevice* device = nullptr;
        ALCcontext* context = nullptr;

        ALuint sourceId = 0;
        ALenum format = 0;
        std::array<ALuint, 2> bufferIds{};

        std::uint32_t sampleSize = 0;
        std::vector<std::uint8_t> data;

#if !defined(__EMSCRIPTEN__)
        std::atomic_bool running{false};
        thread::Thread audioThread;
#endif
    };
}
#endif

#endif // OUZEL_AUDIO_OALAUDIODEVICE_HPP
