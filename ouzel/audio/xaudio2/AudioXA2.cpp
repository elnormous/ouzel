// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_XAUDIO2

#include "AudioXA2.h"
#include "SoundXA2.h"
#include "XAudio27.h"
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
            resourceDeleteSet.clear();
            resources.clear();

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

            if (apiMajorVersion == 2 && apiMinorVersion == 7)
            {
                HRESULT hr = IXAudio2CreateMasteringVoice(xAudio, &masteringVoice);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create XAudio2 mastering voice, error: " << hr;
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
            }

            return Audio::init();
        }

        bool AudioXA2::update()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            if (dirty & DIRTY_LISTENER_POSITION)
            {

            }

            if (dirty & DIRTY_LISTENER_ROTATION)
            {

            }

            dirty = 0;

            return true;
        }

        SoundResource* AudioXA2::createSound()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            SoundResource* sound = new SoundXA2();
            resources.push_back(std::unique_ptr<SoundResource>(sound));
            return sound;
        }

        IXAudio2SourceVoice* AudioXA2::createSourceVoice(const WAVEFORMATEX& sourceFormat)
        {
            IXAudio2SourceVoice* sourceVoice;

            if (apiMajorVersion == 2 && apiMinorVersion == 7)
            {
                HRESULT hr = IXAudio2CreateSourceVoice(xAudio, &sourceVoice, &sourceFormat);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create source voice, error: " << hr;
                    return nullptr;
                }
            }
            else
            {
                HRESULT hr = xAudio->CreateSourceVoice(&sourceVoice, &sourceFormat);
                if (FAILED(hr))
                {
                    Log(Log::Level::ERR) << "Failed to create source voice, error: " << hr;
                    return nullptr;
                }
            }

            return sourceVoice;
        }
    } // namespace audio
} // namespace ouzel

#endif
