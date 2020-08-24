// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_DSAUDIODEVICE_HPP
#define OUZEL_AUDIO_DSAUDIODEVICE_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECTSOUND

#include <atomic>
#include <thread>
#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <mmreg.h>
#include <dsound.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "../AudioDevice.hpp"
#include "DSPointer.hpp"
#include "../../thread/Thread.hpp"

namespace ouzel::audio::directsound
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

        auto& getDirectSound() const noexcept { return directSound; }

    private:
        void run();

        Pointer<IDirectSound8> directSound;

        Pointer<IDirectSoundBuffer> primaryBuffer;
        Pointer<IDirectSoundBuffer8> buffer;
        Pointer<IDirectSoundNotify> notify;
        HANDLE notifyEvents[2] = {nullptr, nullptr};

        std::uint32_t nextBuffer = 0;

        std::uint32_t sampleSize = 0;
        std::vector<std::uint8_t> data;

        std::atomic_bool running{false};
        thread::Thread audioThread;
    };
}
#endif

#endif // OUZEL_AUDIO_DSAUDIODEVICE_HPP
