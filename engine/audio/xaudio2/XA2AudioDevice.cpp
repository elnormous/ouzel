// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "../../core/Setup.h"

#if OUZEL_COMPILE_XAUDIO2

#include <system_error>
#include "XA2AudioDevice.hpp"
#include "XA2ErrorCategory.hpp"
#include "XAudio27.hpp"
#include "../../utils/Log.hpp"
#include "../../utils/Utils.hpp"

#ifndef XAUDIO2_DEBUG_ENGINE
#  define XAUDIO2_DEBUG_ENGINE 0x0001 // Used in XAudio2Create on Windows only
#endif

namespace ouzel::audio::xaudio2
{
    namespace
    {
        const ErrorCategory errorCategory{};
    }

    AudioDevice::AudioDevice(const Settings& settings,
                             const std::function<void(std::uint32_t frames,
                                                      std::uint32_t channels,
                                                      std::uint32_t sampleRate,
                                                      std::vector<float>& samples)>& initDataGetter):
        audio::AudioDevice(Driver::xAudio2, settings, initDataGetter)
    {
        constexpr char* xaudio2dll28 = "xaudio2_8.dll";
        constexpr char* xaudio2dll27 = "xaudio2_7.dll";

        xAudio2Library = LoadLibraryA(xaudio2dll28);

        if (xAudio2Library)
        {
            apiMajorVersion = 2;
            apiMinorVersion = 8;

            using XAudio2CreateProc = HRESULT(__stdcall *)(IXAudio2** ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor);
            const auto xAudio2CreateProc = reinterpret_cast<XAudio2CreateProc>(GetProcAddress(xAudio2Library, "XAudio2Create"));

            if (!xAudio2CreateProc)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to get address of XAudio2Create");

            if (const auto hr = xAudio2CreateProc(&xAudio, 0, XAUDIO2_DEFAULT_PROCESSOR); FAILED(hr))
                throw std::system_error(hr, errorCategory, "Failed to initialize XAudio2");

            if (settings.debugAudio)
            {
                XAUDIO2_DEBUG_CONFIGURATION debugConfiguration;
                debugConfiguration.TraceMask = XAUDIO2_LOG_WARNINGS | XAUDIO2_LOG_DETAIL;
                debugConfiguration.BreakMask = XAUDIO2_LOG_ERRORS;
                debugConfiguration.LogThreadID = FALSE;
                debugConfiguration.LogFileline = FALSE;
                debugConfiguration.LogFunctionName = FALSE;
                debugConfiguration.LogTiming = FALSE;

                xAudio->SetDebugConfiguration(&debugConfiguration);
            }
        }
        else
        {
            logger.log(Log::Level::info) << "Failed to load " << xaudio2dll28;

            xAudio2Library = LoadLibraryA(xaudio2dll27);

            if (xAudio2Library)
            {
                apiMajorVersion = 2;
                apiMinorVersion = 7;
            }
            else
                throw std::system_error(GetLastError(),  std::system_category(), "Failed to load " + std::string(xaudio2dll27));

            UINT32 flags = 0;
            if (settings.debugAudio) flags |= XAUDIO2_DEBUG_ENGINE;

            if (const auto hr = XAudio27CreateProc(&xAudio, flags, XAUDIO2_DEFAULT_PROCESSOR); FAILED(hr))
                throw std::system_error(hr, errorCategory, "Failed to initialize XAudio2");
        }

        WAVEFORMATEX waveFormat;
        waveFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
        waveFormat.nChannels = static_cast<WORD>(channels);
        waveFormat.nSamplesPerSec = sampleRate;
        waveFormat.wBitsPerSample = sizeof(float) * 8;
        waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample / 8);
        waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
        waveFormat.cbSize = 0;

        if (apiMajorVersion == 2 && apiMinorVersion == 7)
        {
            if (const auto hr = IXAudio2CreateMasteringVoice(xAudio, &masteringVoice); FAILED(hr))
                throw std::system_error(hr, errorCategory, "Failed to create XAudio2 mastering voice");

            if (const auto hr = IXAudio2CreateSourceVoice(xAudio, &sourceVoice, &waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this); FAILED(hr))
                throw std::system_error(hr, errorCategory, "Failed to create source voice");
        }
        else
        {
            if (const auto hr = xAudio->CreateMasteringVoice(&masteringVoice); FAILED(hr))
                throw std::system_error(hr, errorCategory, "Failed to create XAudio2 mastering voice");

            if (const auto hr = xAudio->CreateSourceVoice(&sourceVoice, &waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this); FAILED(hr))
                throw std::system_error(hr, errorCategory, "Failed to create source voice");
        }

        sampleFormat = SampleFormat::float32;
    }

    AudioDevice::~AudioDevice()
    {
        if (sourceVoice) sourceVoice->DestroyVoice();
        if (masteringVoice) masteringVoice->DestroyVoice();
        if (xAudio)
        {
            if (apiMajorVersion == 2 && apiMinorVersion == 7) IXAudio2Release(xAudio);
            else xAudio->Release();
        }
        if (xAudio2Library) FreeModule(xAudio2Library);
    }

    void AudioDevice::start()
    {
        getData(bufferSize / (channels * sizeof(float)), data[0]);

        XAUDIO2_BUFFER bufferData;
        bufferData.Flags = 0;
        bufferData.AudioBytes = static_cast<UINT32>(data[0].size());
        bufferData.pAudioData = data[0].data();
        bufferData.PlayBegin = 0;
        bufferData.PlayLength = 0;
        bufferData.LoopBegin = 0;
        bufferData.LoopLength = 0;
        bufferData.LoopCount = 0;
        bufferData.pContext = nullptr;

        if (const auto hr = sourceVoice->SubmitSourceBuffer(&bufferData); FAILED(hr))
            throw std::system_error(hr, errorCategory, "Failed to upload sound data");

        getData(bufferSize / (channels * sizeof(float)), data[1]);
        bufferData.AudioBytes = static_cast<UINT32>(data[1].size());
        bufferData.pAudioData = data[1].data();

        if (const auto hr = sourceVoice->SubmitSourceBuffer(&bufferData); FAILED(hr))
            throw std::system_error(hr, errorCategory, "Failed to upload sound data");

        nextBuffer = 0;

        if (const auto hr = sourceVoice->Start(); FAILED(hr))
            throw std::system_error(hr, errorCategory, "Failed to start consuming sound data");
    }

    void AudioDevice::stop()
    {
        if (const auto hr = sourceVoice->Stop(); FAILED(hr))
            throw std::system_error(hr, errorCategory, "Failed to stop consuming sound data");

        if (const auto hr = sourceVoice->FlushSourceBuffers(); FAILED(hr))
            throw std::system_error(hr, errorCategory, "Failed to flush sound data");
    }

    void AudioDevice::OnVoiceProcessingPassStart(UINT32)
    {
    }

    void AudioDevice::OnVoiceProcessingPassEnd()
    {
    }

    void AudioDevice::OnStreamEnd()
    {
    }

    void AudioDevice::OnBufferStart(void*)
    {
    }

    void AudioDevice::OnBufferEnd(void*)
    {
        getData(bufferSize / (channels * sizeof(float)), data[nextBuffer]);

        XAUDIO2_BUFFER bufferData;
        bufferData.Flags = 0;
        bufferData.AudioBytes = static_cast<UINT32>(data[nextBuffer].size());
        bufferData.pAudioData = data[nextBuffer].data();
        bufferData.PlayBegin = 0;
        bufferData.PlayLength = 0;
        bufferData.LoopBegin = 0;
        bufferData.LoopLength = 0;
        bufferData.LoopCount = 0;
        bufferData.pContext = nullptr;

        if (const auto hr = sourceVoice->SubmitSourceBuffer(&bufferData); FAILED(hr))
            throw std::system_error(hr, errorCategory, "Failed to upload sound data");

        nextBuffer = (nextBuffer == 0) ? 1 : 0;
    }

    void AudioDevice::OnLoopEnd(void*)
    {
    }

    void AudioDevice::OnVoiceError(void*, HRESULT error)
    {
        throw std::runtime_error("Xaudio2 voice error: " + std::to_string(error));
    }
}
#endif
