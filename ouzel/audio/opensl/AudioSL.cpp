// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioSL.h"
#include "SoundDataSL.h"
#include "SoundSL.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace audio
    {
        AudioSL::AudioSL():
            Audio(Driver::OPENSL)
        {
        }

        AudioSL::~AudioSL()
        {
            if (outputMixObject)
            {
                (*outputMixObject)->Destroy(outputMixObject);
            }

            if (engineObject)
            {
                (*engineObject)->Destroy(engineObject);
            }
        }

        void AudioSL::free()
        {
            Audio::free();

            if (outputMixObject)
            {
                (*outputMixObject)->Destroy(outputMixObject);
                outputMixObject = nullptr;
            }

            if (engineObject)
            {
                (*engineObject)->Destroy(engineObject);
                engineObject = nullptr;
                engine = nullptr;
            }
        }

        bool AudioSL::init()
        {
            if (!Audio::init())
            {
                return false;
            }

            free();

            const SLuint32 engineMixIIDCount = 1;
            const SLInterfaceID engineMixIID = SL_IID_ENGINE;
            const SLboolean engineMixReq = SL_BOOLEAN_TRUE;

            if (slCreateEngine(&engineObject, 0, NULL, engineMixIIDCount, &engineMixIID, &engineMixReq) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to create OpenSL engine object";
                return false;
            }

            if ((*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to create OpenSL engine object";
                return false;
            }

            if ((*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engine) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to get OpenSL engine";
                return false;
            }

            if ((*engine)->CreateOutputMix(engine, &outputMixObject, 0, nullptr, nullptr) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to create OpenSL output mix";
                return false;
            }

            if ((*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to create OpenSL output mix";
                return false;
            }

            ready = true;

            return true;
        }

        SoundDataPtr AudioSL::createSoundData()
        {
            SoundDataPtr soundData = std::make_shared<SoundDataSL>();
            return soundData;
        }

        SoundPtr AudioSL::createSound()
        {
            SoundPtr sound = std::make_shared<SoundSL>();
            return sound;
        }
    } // namespace audio
} // namespace ouzel
