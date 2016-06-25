// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioXA2.h"
#include "SoundDataXA2.h"
#include "SoundXA2.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace audio
    {
        AudioXA2::AudioXA2()
        {
        }

        AudioXA2::~AudioXA2()
        {
            if (masteringVoice) masteringVoice->DestroyVoice();
            if (xAudio) xAudio->Release();
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
        }

        bool AudioXA2::init()
        {
            if (!Audio::init())
            {
                return false;
            }

            ready = false;

            if (FAILED(XAudio2Create(&xAudio)))
            {
                log("Failed to initialize XAudio");
                return false;
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
