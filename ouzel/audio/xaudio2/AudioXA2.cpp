// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_XAUDIO2

#include "AudioXA2.h"
#include "XAudio27.h"
#include "audio/SoundResource.h"
#include "utils/Log.h"

static const char* XAUDIO2_DLL_28 = "xaudio2_8.dll";
static const char* XAUDIO2_DLL_27 = "xaudio2_7.dll";

typedef HRESULT(__stdcall *XAudio2CreateProc)(IXAudio2** ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor);

namespace ouzel
{
    namespace audio
    {
        AudioXA2::AudioXA2():
            Audio(Driver::XAUDIO2)
        {
        }

        AudioXA2::~AudioXA2()
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

        bool AudioXA2::init()
        {
            xAudio2Library = LoadLibraryA(XAUDIO2_DLL_28);

            if (xAudio2Library)
            {
                apiMajorVersion = 2;
                apiMinorVersion = 8;

                XAudio2CreateProc xAudio2CreateProc = reinterpret_cast<XAudio2CreateProc>(GetProcAddress(xAudio2Library, "XAudio2Create"));

                if (!xAudio2CreateProc)
                {
                    Log(Log::Level::ERR) << "Failed to get address of XAudio2Create";
                    return false;
                }

                HRESULT hr = xAudio2CreateProc(&xAudio, 0, XAUDIO2_DEFAULT_PROCESSOR);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to initialize XAudio2, error: " << hr;
                    return false;
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
                {
                    Log(Log::Level::ERR) << "Failed to load " << XAUDIO2_DLL_27;
                    return false;
                }

                HRESULT hr = XAudio27CreateProc(&xAudio, 0, XAUDIO2_DEFAULT_PROCESSOR);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to initialize XAudio2, error: " << hr;
                    return false;
                }
            }

            WAVEFORMATEX waveFormat;
            waveFormat.wFormatTag = WAVE_FORMAT_PCM;
            waveFormat.nChannels = channels;
            waveFormat.nSamplesPerSec = samplesPerSecond;
            waveFormat.wBitsPerSample = 16;
            waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample / 8);
            waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
            waveFormat.cbSize = 0;

            if (apiMajorVersion == 2 && apiMinorVersion == 7)
            {
                HRESULT hr = IXAudio2CreateMasteringVoice(xAudio, &masteringVoice);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create XAudio2 mastering voice, error: " << hr;
                    return false;
                }

                hr = IXAudio2CreateSourceVoice(xAudio, &sourceVoice, &waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create source voice, error: " << hr;
                    return false;
                }
            }
            else
            {
                HRESULT hr = xAudio->CreateMasteringVoice(&masteringVoice);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create XAudio2 mastering voice, error: " << hr;
                    return false;
                }

                hr = xAudio->CreateSourceVoice(&sourceVoice, &waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create source voice, error: " << hr;
                    return false;
                }
            }

            data[0] = getData(bufferSize);

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
            {
                Log(Log::Level::ERR) << "Failed to upload sound data, error: " << hr;
                return false;
            }

            data[1] = getData(bufferSize);
            bufferData.AudioBytes = static_cast<UINT32>(data[1].size());
            bufferData.pAudioData = data[1].data();

            hr = sourceVoice->SubmitSourceBuffer(&bufferData);
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to upload sound data, error: " << hr;
                return false;
            }

            nextBuffer = 0;

            hr = sourceVoice->Start();
            if (FAILED(hr))
            {
                Log(Log::Level::ERR) << "Failed to start consuming sound data, error: " << hr;
                return false;
            }

            return Audio::init();
        }

        bool AudioXA2::update()
        {
            Audio::update();

            std::lock_guard<std::mutex> lock(resourceMutex);
            dirty = 0;

            return true;
        }

        void AudioXA2::OnVoiceProcessingPassStart(UINT32)
        {
        }
        void AudioXA2::OnVoiceProcessingPassEnd()
        {
        }
        void AudioXA2::OnStreamEnd()
        {
        }
        void AudioXA2::OnBufferStart(void*)
        {
        }
        void AudioXA2::OnBufferEnd(void*)
        {
            data[nextBuffer] = getData(bufferSize);

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
            {
                Log(Log::Level::ERR) << "Failed to upload sound data, error: " << hr;
            }

            nextBuffer = (nextBuffer == 0) ? 1 : 0;
        }
        void AudioXA2::OnLoopEnd(void*)
        {
        }
        void AudioXA2::OnVoiceError(void*, HRESULT error)
        {
            Log() << "Xaudio2 voice error: " << error;
        }

    } // namespace audio
} // namespace ouzel

#endif
