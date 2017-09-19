// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_XAUDIO2

#include <atomic>
#include <thread>
#include <xaudio2.h>
#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioDeviceXA2: public AudioDevice, public IXAudio2VoiceCallback
        {
            friend Audio;
        public:
            virtual ~AudioDeviceXA2();

        protected:
            AudioDeviceXA2();
            virtual bool init(bool debugAudio) override;

            void run();

            virtual void OnVoiceProcessingPassStart(UINT32 bytesRequired) override;
            virtual void OnVoiceProcessingPassEnd() override;
            virtual void OnStreamEnd() override;
            virtual void OnBufferStart(void* bufferContext) override;
            virtual void OnBufferEnd(void* bufferContext) override;
            virtual void OnLoopEnd(void* bufferContext) override;
            virtual void OnVoiceError(void* bufferContext, HRESULT error) override;

            HMODULE xAudio2Library = nullptr;

            IXAudio2* xAudio = nullptr;
            IXAudio2MasteringVoice* masteringVoice = nullptr;
            IXAudio2SourceVoice* sourceVoice = nullptr;

            std::vector<uint8_t> data[2];
            uint32_t nextBuffer = 0;

            std::atomic<bool> running;
            std::thread audioThread;
            std::condition_variable fillDataCondition;
            bool fillData = false;
            std::mutex fillDataMutex;
        };
    } // namespace audio
} // namespace ouzel

#endif
