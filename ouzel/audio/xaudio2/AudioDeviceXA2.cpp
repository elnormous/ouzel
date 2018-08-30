// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_XAUDIO2

#include "AudioDeviceXA2.hpp"
#include "XAudio27.hpp"
#include "core/Engine.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"

static const char* XAUDIO2_DLL_28 = "xaudio2_8.dll";
static const char* XAUDIO2_DLL_27 = "xaudio2_7.dll";

typedef HRESULT(__stdcall *XAudio2CreateProc)(IXAudio2** ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor);

namespace ouzel
{
    namespace audio
    {
        AudioDeviceXA2::AudioDeviceXA2(bool debugAudio):
            AudioDevice(Audio::Driver::XAUDIO2), running(true)
        {
            xAudio2Library = LoadLibraryA(XAUDIO2_DLL_28);

            if (xAudio2Library)
            {
                apiMajorVersion = 2;
                apiMinorVersion = 8;

                XAudio2CreateProc xAudio2CreateProc = reinterpret_cast<XAudio2CreateProc>(GetProcAddress(xAudio2Library, "XAudio2Create"));

                if (!xAudio2CreateProc)
                    throw SystemError("Failed to get address of XAudio2Create");

                HRESULT hr = xAudio2CreateProc(&xAudio, 0, XAUDIO2_DEFAULT_PROCESSOR);
                if (FAILED(hr))
                    throw SystemError("Failed to initialize XAudio2, error: " + std::to_string(hr));

                if (debugAudio)
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
                Log(Log::Level::INFO) << "Failed to load " << XAUDIO2_DLL_28;

                xAudio2Library = LoadLibraryA(XAUDIO2_DLL_27);

                if (xAudio2Library)
                {
                    apiMajorVersion = 2;
                    apiMinorVersion = 7;
                }
                else
                    throw SystemError("Failed to load " + std::string(XAUDIO2_DLL_27));

                const UINT XAUDIO2_DEBUG_ENGINE = 0x0001;

                UINT32 flags = 0;
                if (debugAudio) flags |= XAUDIO2_DEBUG_ENGINE;

                HRESULT hr = XAudio27CreateProc(&xAudio, flags, XAUDIO2_DEFAULT_PROCESSOR);
                if (FAILED(hr))
                    throw SystemError("Failed to initialize XAudio2, error: " + std::to_string(hr));
            }

            WAVEFORMATEX waveFormat;
            waveFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
            waveFormat.nChannels = channels;
            waveFormat.nSamplesPerSec = sampleRate;
            waveFormat.wBitsPerSample = 32;
            waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample / 8);
            waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
            waveFormat.cbSize = 0;

            if (apiMajorVersion == 2 && apiMinorVersion == 7)
            {
                HRESULT hr = IXAudio2CreateMasteringVoice(xAudio, &masteringVoice);
                if (FAILED(hr))
                    throw SystemError("Failed to create XAudio2 mastering voice, error: " + std::to_string(hr));

                hr = IXAudio2CreateSourceVoice(xAudio, &sourceVoice, &waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this);
                if (FAILED(hr))
                    throw SystemError("Failed to create source voice, error: " + std::to_string(hr));
            }
            else
            {
                HRESULT hr = xAudio->CreateMasteringVoice(&masteringVoice);
                if (FAILED(hr))
                    throw SystemError("Failed to create XAudio2 mastering voice, error: " + std::to_string(hr));

                hr = xAudio->CreateSourceVoice(&sourceVoice, &waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this);
                if (FAILED(hr))
                    throw SystemError("Failed to create source voice, error: " + std::to_string(hr));
            }

            sampleFormat = Audio::SampleFormat::FLOAT32;

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

            HRESULT hr = sourceVoice->SubmitSourceBuffer(&bufferData);
            if (FAILED(hr))
                throw SystemError("Failed to upload sound data, error: " + std::to_string(hr));

            getData(bufferSize / (channels * sizeof(float)), data[1]);
            bufferData.AudioBytes = static_cast<UINT32>(data[1].size());
            bufferData.pAudioData = data[1].data();

            hr = sourceVoice->SubmitSourceBuffer(&bufferData);
            if (FAILED(hr))
                throw SystemError("Failed to upload sound data, error: " + std::to_string(hr));

            nextBuffer = 0;

            hr = sourceVoice->Start();
            if (FAILED(hr))
                throw SystemError("Failed to start consuming sound data, error: " + std::to_string(hr));

            audioThread = std::thread(&AudioDeviceXA2::run, this);
        }

        AudioDeviceXA2::~AudioDeviceXA2()
        {
            running = false;

            if (audioThread.joinable())
            {
                {
                    std::unique_lock<std::mutex> lock(fillDataMutex);
                    fillDataCondition.notify_all();
                }

                audioThread.join();
            }

            if (sourceVoice) sourceVoice->DestroyVoice();
            if (masteringVoice) masteringVoice->DestroyVoice();
            if (xAudio)
            {
                if (apiMajorVersion == 2 && apiMinorVersion == 7) IXAudio2Release(xAudio);
                else xAudio->Release();
            }
            if (xAudio2Library) FreeModule(xAudio2Library);
        }

        void AudioDeviceXA2::run()
        {
            setCurrentThreadName("Audio");

            for (;;)
            {
                try
                {
                    std::unique_lock<std::mutex> lock(fillDataMutex);
                    while (!fillData && running) fillDataCondition.wait(lock);

                    if (!running) break;

                    process();

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

                    HRESULT hr = sourceVoice->SubmitSourceBuffer(&bufferData);
                    if (FAILED(hr))
                        throw SystemError("Failed to upload sound data, error: " + std::to_string(hr));

                    nextBuffer = (nextBuffer == 0) ? 1 : 0;

                    fillData = false;
                }
                catch (const std::exception& e)
                {
                    Log(Log::Level::ERR) << e.what();
                }
                catch (...)
                {
                    Log(Log::Level::ERR) << "Unknown error occurred";
                }
            }
        }

        void AudioDeviceXA2::OnVoiceProcessingPassStart(UINT32)
        {
        }

        void AudioDeviceXA2::OnVoiceProcessingPassEnd()
        {
        }

        void AudioDeviceXA2::OnStreamEnd()
        {
        }

        void AudioDeviceXA2::OnBufferStart(void*)
        {
        }

        void AudioDeviceXA2::OnBufferEnd(void*)
        {
            std::unique_lock<std::mutex> lock(fillDataMutex);
            fillData = true;
            fillDataCondition.notify_all();
        }

        void AudioDeviceXA2::OnLoopEnd(void*)
        {
        }

        void AudioDeviceXA2::OnVoiceError(void*, HRESULT error)
        {
            throw SystemError("Xaudio2 voice error: " + std::to_string(error));
        }
    } // namespace audio
} // namespace ouzel

#endif
