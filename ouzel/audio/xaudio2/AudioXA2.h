// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_XAUDIO2

#include <xaudio2.h>
#include "audio/Audio.h"

namespace ouzel
{
    namespace audio
    {
        class AudioXA2: public Audio, public IXAudio2VoiceCallback
        {
            friend Engine;
        public:
            virtual ~AudioXA2();

        protected:
            AudioXA2();
            virtual bool init() override;

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
        };
    } // namespace audio
} // namespace ouzel

#endif
