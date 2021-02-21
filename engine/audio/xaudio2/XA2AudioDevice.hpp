// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_XA2AUDIODEVICE_HPP
#define OUZEL_AUDIO_XA2AUDIODEVICE_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_XAUDIO2

#include <vector>
#include <xaudio2.h>
#include "../AudioDevice.hpp"

namespace ouzel::audio::xaudio2
{
    class AudioDevice final: public audio::AudioDevice, public IXAudio2VoiceCallback
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

        void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 bytesRequired) final;
        void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() final;
        void STDMETHODCALLTYPE OnStreamEnd() final;
        void STDMETHODCALLTYPE OnBufferStart(void* bufferContext) final;
        void STDMETHODCALLTYPE OnBufferEnd(void* bufferContext) final;
        void STDMETHODCALLTYPE OnLoopEnd(void* bufferContext) final;
        void STDMETHODCALLTYPE OnVoiceError(void* bufferContext, HRESULT error) final;

        HMODULE xAudio2Library = nullptr;

        IXAudio2* xAudio = nullptr;
        IXAudio2MasteringVoice* masteringVoice = nullptr;
        IXAudio2SourceVoice* sourceVoice = nullptr;

        std::vector<std::uint8_t> data[2];
        std::uint32_t nextBuffer = 0;
    };
}
#endif

#endif // OUZEL_AUDIO_XA2AUDIODEVICE_HPP
