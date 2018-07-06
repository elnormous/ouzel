// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_XAUDIO2

#include <atomic>
#include <xaudio2.h>
#include "audio/AudioDevice.hpp"
#include "thread/Condition.hpp"
#include "thread/Thread.hpp"

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
            AudioDeviceXA2(bool debugAudio);

            void run();

            virtual void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 bytesRequired) override;
            virtual void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override;
            virtual void STDMETHODCALLTYPE OnStreamEnd() override;
            virtual void STDMETHODCALLTYPE OnBufferStart(void* bufferContext) override;
            virtual void STDMETHODCALLTYPE OnBufferEnd(void* bufferContext) override;
            virtual void STDMETHODCALLTYPE OnLoopEnd(void* bufferContext) override;
            virtual void STDMETHODCALLTYPE OnVoiceError(void* bufferContext, HRESULT error) override;

        private:
            HMODULE xAudio2Library = nullptr;

            IXAudio2* xAudio = nullptr;
            IXAudio2MasteringVoice* masteringVoice = nullptr;
            IXAudio2SourceVoice* sourceVoice = nullptr;

            std::vector<uint8_t> data[2];
            uint32_t nextBuffer = 0;

            std::atomic<bool> running;
            Thread audioThread;
            Condition fillDataCondition;
            bool fillData = false;
            Mutex fillDataMutex;
        };
    } // namespace audio
} // namespace ouzel

#endif
