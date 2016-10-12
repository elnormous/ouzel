// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#include "AudioAL.h"
#include "SoundDataAL.h"
#include "SoundAL.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace audio
    {
        bool AudioAL::checkALCError(bool logError)
        {
            ALCenum error = alcGetError(device);

            if (error != ALC_NO_ERROR)
            {
                if (logError)
                {
                    const char* errorStr = "Unknown error";

                    switch (error)
                    {
                        case ALC_INVALID_DEVICE: errorStr = "ALC_INVALID_DEVICE"; break;
                        case ALC_INVALID_CONTEXT: errorStr = "ALC_INVALID_CONTEXT"; break;
                        case ALC_INVALID_ENUM: errorStr = "ALC_INVALID_ENUM"; break;
                        case ALC_INVALID_VALUE: errorStr = "ALC_INVALID_VALUE"; break;
                        case ALC_OUT_OF_MEMORY: errorStr = "ALC_OUT_OF_MEMORY"; break;
                    }

                    Log(Log::Level::ERR) << "OpenAL error: " << errorStr << "(" << error << ")";
                }

                return true;
            }

            return false;
        }

        bool AudioAL::checkOpenALError(bool logError)
        {
            ALenum error = alGetError();

            if (error != AL_NO_ERROR)
            {
                if (logError)
                {
                    const char* errorStr = "Unknown error";

                    switch (error)
                    {
                        case AL_INVALID_NAME: errorStr = "AL_INVALID_NAME"; break;
                        case AL_INVALID_ENUM: errorStr = "GL_INVALID_ENUM"; break;
                        case AL_INVALID_VALUE: errorStr = "GL_INVALID_VALUE"; break;
                        case AL_INVALID_OPERATION: errorStr = "GL_INVALID_OPERATION"; break;
                        case AL_OUT_OF_MEMORY: errorStr = "GL_OUT_OF_MEMORY"; break;
                    }

                    Log(Log::Level::ERR) << "OpenAL error: " << errorStr << " (" << error << ")";
                }

                return true;
            }

            return false;
        }

        AudioAL::AudioAL():
            Audio(Driver::OPENAL)
        {
        }

        AudioAL::~AudioAL()
        {
            if (context)
            {
                alcDestroyContext(context);
            }

            if (device)
            {
                alcCloseDevice(device);
            }
        }

        void AudioAL::free()
        {
            Audio::free();

            if (context)
            {
                alcDestroyContext(context);
                context = nullptr;

                if (checkOpenALError())
                {
                    Log(Log::Level::ERR) << "Failed to delete OpenAL context";
                }
            }

            if (device)
            {
                alcCloseDevice(device);
                device = nullptr;

                if (checkOpenALError())
                {
                    Log(Log::Level::ERR) << "Failed to close OpenAL device";
                }
            }
        }

        bool AudioAL::init()
        {
            if (!Audio::init())
            {
                return false;
            }

            free();

            const ALCchar* deviceName = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);

            if (deviceName)
            {
                Log(Log::Level::INFO) << "Using " << deviceName << " for audio";
            }

            device = alcOpenDevice(deviceName);

            if (!device || checkALCError())
            {
                Log(Log::Level::ERR) << "Failed to create OpenAL device";
                return false;
            }

            context = alcCreateContext(device, nullptr);

            if (checkALCError())
            {
                Log(Log::Level::ERR) << "Failed to create OpenAL context";
                return false;
            }

            alcMakeContextCurrent(context);

            if (checkALCError())
            {
                Log(Log::Level::ERR) << "Failed to make OpenAL context current";
                return false;
            }

            ready = true;

            return true;
        }

        SoundDataPtr AudioAL::createSoundData()
        {
            SoundDataPtr soundData = std::make_shared<SoundDataAL>();
            return soundData;
        }

        SoundPtr AudioAL::createSound()
        {
            SoundPtr sound = std::make_shared<SoundAL>();
            return sound;
        }
    } // namespace audio
} // namespace ouzel
