// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICEXA2_HPP
#define OUZEL_AUDIO_AUDIODEVICEXA2_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_XAUDIO2

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <xaudio2.h>
#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioDeviceXA2 final: public AudioDevice, public IXAudio2VoiceCallback
        {
        public:
            AudioDeviceXA2(bool debugAudio);
            ~AudioDeviceXA2();

        private:
            void run();

            void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 bytesRequired) override;
            void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override;
            void STDMETHODCALLTYPE OnStreamEnd() override;
            void STDMETHODCALLTYPE OnBufferStart(void* bufferContext) override;
            void STDMETHODCALLTYPE OnBufferEnd(void* bufferContext) override;
            void STDMETHODCALLTYPE OnLoopEnd(void* bufferContext) override;
            void STDMETHODCALLTYPE OnVoiceError(void* bufferContext, HRESULT error) override;

            HMODULE xAudio2Library = nullptr;

            IXAudio2* xAudio = nullptr;
            IXAudio2MasteringVoice* masteringVoice = nullptr;
            IXAudio2SourceVoice* sourceVoice = nullptr;

            std::vector<uint8_t> data[2];
            uint32_t nextBuffer = 0;

            std::atomic_bool running;
            std::thread audioThread;
            std::condition_variable fillDataCondition;
            bool fillData = false;
            std::mutex fillDataMutex;
        };
    } // namespace audio
} // namespace ouzel

#endif

#endif // OUZEL_AUDIO_AUDIODEVICEXA2_HPP
