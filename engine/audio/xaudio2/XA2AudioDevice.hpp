// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_XA2AUDIODEVICE_HPP
#define OUZEL_AUDIO_XA2AUDIODEVICE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_XAUDIO2

#include <vector>
#include <xaudio2.h>
#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace xaudio2
        {
            class AudioDevice final: public audio::AudioDevice, public IXAudio2VoiceCallback
            {
            public:
                AudioDevice(uint32_t initBufferSize,
                            uint32_t initSampleRate,
                            uint32_t initChannels,
                            const std::function<void(uint32_t frames,
                                                     uint32_t channels,
                                                     uint32_t sampleRate,
                                                     std::vector<float>& samples)>& initDataGetter,
                            bool debugAudio);
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

                std::vector<uint8_t> data[2];
                uint32_t nextBuffer = 0;
            };
        } // namespace xaudio2
    } // namespace audio
} // namespace ouzel

#endif

#endif // OUZEL_AUDIO_XA2AUDIODEVICE_HPP
