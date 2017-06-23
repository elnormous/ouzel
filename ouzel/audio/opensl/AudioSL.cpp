// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENSL

#include "AudioSL.h"
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
            resourceDeleteSet.clear();
            resources.clear();

            if (outputMixObject)
            {
                (*outputMixObject)->Destroy(outputMixObject);
            }

            if (engineObject)
            {
                (*engineObject)->Destroy(engineObject);
            }
        }

        bool AudioSL::init()
        {
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

            return Audio::init();
        }

        SoundResource* AudioSL::createSound()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            SoundResource* sound = new SoundSL();
            resources.push_back(std::unique_ptr<SoundResource>(sound));
            return sound;
        }
    } // namespace audio
} // namespace ouzel

#endif
