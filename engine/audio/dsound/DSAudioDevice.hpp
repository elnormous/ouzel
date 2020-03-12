// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_DSAUDIODEVICE_HPP
#define OUZEL_AUDIO_DSAUDIODEVICE_HPP

#include "core/Setup.h"

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

#include "audio/AudioDevice.hpp"
#include "audio/dsound/DSPointer.hpp"
#include "utils/Thread.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace directsound
        {
            class AudioDevice final: public audio::AudioDevice
            {
            public:
                AudioDevice(uint32_t initBufferSize,
                            uint32_t initSampleRate,
                            uint32_t initChannels,
                            const std::function<void(uint32_t frames,
                                                     uint32_t channels,
                                                     uint32_t sampleRate,
                                                     std::vector<float>& samples)>& initDataGetter);
                ~AudioDevice() override;

                void start() final;
                void stop() final;

                inline auto& getDirectSound() const noexcept { return directSound; }

            private:
                void run();

                Pointer<IDirectSound8> directSound;

                Pointer<IDirectSoundBuffer> primaryBuffer;
                Pointer<IDirectSoundBuffer8> buffer;
                Pointer<IDirectSoundNotify> notify;
                HANDLE notifyEvents[2] = {nullptr, nullptr};

                uint32_t nextBuffer = 0;

                uint32_t sampleSize = 0;
                std::vector<uint8_t> data;

                std::atomic_bool running{false};
                Thread audioThread;
            };
        } // namespace directsound
    } // namespace audio
} // namespace ouzel

#endif

#endif // OUZEL_AUDIO_DSAUDIODEVICE_HPP
