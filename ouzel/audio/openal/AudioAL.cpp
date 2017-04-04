// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#include "AudioAL.h"
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
                    const char* errorStr;

                    switch (error)
                    {
                        case AL_INVALID_NAME: errorStr = "AL_INVALID_NAME"; break;
                        case AL_INVALID_ENUM: errorStr = "GL_INVALID_ENUM"; break;
                        case AL_INVALID_VALUE: errorStr = "GL_INVALID_VALUE"; break;
                        case AL_INVALID_OPERATION: errorStr = "GL_INVALID_OPERATION"; break;
                        case AL_OUT_OF_MEMORY: errorStr = "GL_OUT_OF_MEMORY"; break;
                        default: errorStr = "Unknown error"; break;
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
            resourceDeleteSet.clear();
            resources.clear();

            if (context)
            {
                alcMakeContextCurrent(nullptr);
                alcDestroyContext(context);
            }

            if (device)
            {
                alcCloseDevice(device);
            }
        }

        bool AudioAL::init()
        {
            if (!Audio::init())
            {
                return false;
            }

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

            return true;
        }

        bool AudioAL::update()
        {
            alcMakeContextCurrent(context);

            if (checkALCError())
            {
                Log(Log::Level::ERR) << "Failed to make OpenAL context current";
                return false;
            }

            Audio::update();

            return true;
        }

        void AudioAL::setListenerPosition(const Vector3& newPosition)
        {
            Audio::setListenerPosition(newPosition);
            alListenerfv(AL_POSITION, listenerPosition.v);
        }

        void AudioAL::setListenerRotation(const Quaternion& newRotation)
        {
            Audio::setListenerRotation(newRotation);

            Vector3 forwardVector = listenerRotation.getForwardVector();
            Vector3 upVector = listenerRotation.getUpVector();

            float values[] = {
                forwardVector.v[0],
                forwardVector.v[1],
                -forwardVector.v[2],
                upVector.v[0],
                upVector.v[1],
                -upVector.v[2]
            };

            alListenerfv(AL_ORIENTATION, values);
        }

        SoundResource* AudioAL::createSound()
        {
            SoundResource* sound = new SoundAL();
            resources.push_back(std::unique_ptr<SoundResource>(sound));
            return sound;
        }
    } // namespace audio
} // namespace ouzel
