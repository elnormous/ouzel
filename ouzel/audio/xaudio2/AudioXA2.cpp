// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioXA2.h"
#include "SoundDataXA2.h"
#include "SoundXA2.h"
#include "XAudio27.h"
#include "utils/Utils.h"

typedef HRESULT (*XAudio2CreateProc)(IXAudio2** ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor);

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
            if (masteringVoice) masteringVoice->DestroyVoice();
            if (xAudio)
            {
                if (apiMajorVersion == 2 && apiMinorVersion == 7) IXAudio2Release(xAudio);
                else xAudio->Release();
            }
            if (xAudio2Library) FreeModule(xAudio2Library);
        }

        void AudioXA2::free()
        {
            Audio::free();

            if (masteringVoice)
            {
                masteringVoice->DestroyVoice();
                masteringVoice = nullptr;
            }

            if (xAudio)
            {
                if (apiMajorVersion == 2 && apiMinorVersion == 7) IXAudio2Release(xAudio);
                else xAudio->Release();
                xAudio = nullptr;
            }

            if (xAudio2Library)
            {
                FreeModule(xAudio2Library);
                xAudio2Library = nullptr;
            }
        }

        bool AudioXA2::init()
        {
            if (!Audio::init())
            {
                return false;
            }

            free();

			const char* library28 = "xaudio2_8.dll";

            xAudio2Library = LoadLibraryA(library28);

            if (xAudio2Library)
            {
                apiMajorVersion = 2;
                apiMinorVersion = 8;

                XAudio2CreateProc xAudio2CreateProc = reinterpret_cast<XAudio2CreateProc>(GetProcAddress(xAudio2Library, "XAudio2Create"));

                if (!xAudio2CreateProc)
                {
                    log(LOG_LEVEL_ERROR, "Failed to get address of XAudio2Create");
                    return false;
                }

                if (FAILED(xAudio2CreateProc(&xAudio, 0, XAUDIO2_DEFAULT_PROCESSOR)))
                {
                    log(LOG_LEVEL_ERROR, "Failed to initialize XAudio2");
                    return false;
                }
			}
			else
            {
                log(LOG_LEVEL_INFO, "Failed to load %s", library28);

                const char* library27 = "xaudio2_7.dll";

                xAudio2Library = LoadLibraryA(library27);

                if (xAudio2Library)
                {
                    apiMajorVersion = 2;
                    apiMinorVersion = 7;
                }
                else
                {
                    log(LOG_LEVEL_ERROR, "Failed to load %s", library27);
                    return false;
                }

                if (FAILED(XAudio27CreateProc(&xAudio, 0, XAUDIO2_DEFAULT_PROCESSOR)))
                {
                    log(LOG_LEVEL_ERROR, "Failed to initialize XAudio2");
                    return false;
                }
            }

            if (apiMajorVersion == 2 && apiMinorVersion == 7)
            {
                if (FAILED(IXAudio2CreateMasteringVoice(xAudio, &masteringVoice)))
                {
                    log(LOG_LEVEL_ERROR, "Failed to create XAudio2 mastering voice");
                    return false;
                }
            }
            else if (FAILED(xAudio->CreateMasteringVoice(&masteringVoice)))
            {
                log(LOG_LEVEL_ERROR, "Failed to create XAudio2 mastering voice");
                return false;
            }

            ready = true;

            return true;
        }

        SoundDataPtr AudioXA2::createSoundData()
        {
            SoundDataPtr soundData(new SoundDataXA2());
            return soundData;
        }

        SoundPtr AudioXA2::createSound()
        {
            SoundPtr sound(new SoundXA2());
            return sound;
        }

        IXAudio2SourceVoice* AudioXA2::createSourceVoice(const WAVEFORMATEX& sourceFormat)
        {
            IXAudio2SourceVoice* sourceVoice;

            if (apiMajorVersion == 2 && apiMinorVersion == 7)
            {
                if (FAILED(IXAudio2CreateSourceVoice(xAudio, &sourceVoice, &sourceFormat)))
                {
                    log(LOG_LEVEL_ERROR, "Failed to create source voice");
                    return nullptr;
                }
            }
            else if (FAILED(xAudio->CreateSourceVoice(&sourceVoice, &sourceFormat)))
            {
                log(LOG_LEVEL_ERROR, "Failed to create source voice");
                return nullptr;
            }

            return sourceVoice;
        }
    } // namespace audio
} // namespace ouzel
