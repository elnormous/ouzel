// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioXA2.h"
#include "SoundDataXA2.h"
#include "SoundXA2.h"
#include "utils/Utils.h"

typedef HRESULT (*XAudio2CreateProc)(IXAudio2** ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor);
HRESULT XAudio2CreateProc27(IXAudio2** ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor);

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
            if (xAudio) xAudio->Release();
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
                xAudio->Release();
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

#ifdef DEBUG
			std::string library = "xaudiod2_8.dll";
#else
			std::string library = "xaudio2_8.dll";
#endif

			HMODULE xAudio2Library = LoadLibraryA(library.c_str());


			if (xAudio2Library)
			{
				XAudio2CreateProc xAudio2CreateProc = reinterpret_cast<XAudio2CreateProc>(GetProcAddress(xAudio2Library, "XAudio2Create"));

				if (!xAudio2CreateProc)
				{
					log(LOG_LEVEL_ERROR, "Failed to get address of XAudio2Create");
					return false;
				}

				if (FAILED(xAudio2CreateProc(&xAudio, 0, XAUDIO2_DEFAULT_PROCESSOR)))
				{
					log(LOG_LEVEL_ERROR, "Failed to initialize XAudio");
					return false;
				}
			}
			else
            {
                log(LOG_LEVEL_INFO, "Failed to load %s", library.c_str());

#ifdef DEBUG
				library = "xaudiod2_7.dll";
#else
				library = "xaudio2_7.dll";
#endif

				xAudio2Library = LoadLibraryA(library.c_str());

                if (!xAudio2Library)
                {
					log(LOG_LEVEL_ERROR, "Failed to load %s", library.c_str());
                    return false;
                }

				if (FAILED(XAudio2CreateProc27(&xAudio, 0, XAUDIO2_DEFAULT_PROCESSOR)))
				{
					log(LOG_LEVEL_ERROR, "Failed to initialize XAudio");
					return false;
				}
            }

            if (FAILED(xAudio->CreateMasteringVoice(&masteringVoice)))
            {
                xAudio->Release();
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
    } // namespace audio
} // namespace ouzel
