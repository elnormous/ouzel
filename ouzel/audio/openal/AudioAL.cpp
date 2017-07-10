// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENAL

#include "AudioAL.h"
#include "audio/SoundResource.h"
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
                        case AL_INVALID_ENUM: errorStr = "AL_INVALID_ENUM"; break;
                        case AL_INVALID_VALUE: errorStr = "AL_INVALID_VALUE"; break;
                        case AL_INVALID_OPERATION: errorStr = "AL_INVALID_OPERATION"; break;
                        case AL_OUT_OF_MEMORY: errorStr = "AL_OUT_OF_MEMORY"; break;
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
            std::fill(std::begin(buffers), std::end(buffers), 0);
        }

        AudioAL::~AudioAL()
        {
            resourceDeleteSet.clear();
            resources.clear();

            if (sourceId)
            {
                alSourceStop(sourceId);
                alSourcei(sourceId, AL_BUFFER, 0);
                alDeleteSources(1, &sourceId);

                if (AudioAL::checkOpenALError())
                {
                    Log(Log::Level::ERR) << "Failed to delete OpenAL source";
                }
            }

            for (ALuint buffer : buffers)
            {
                if (buffer)
                {
                    alDeleteBuffers(1, &buffer);

                    if (AudioAL::checkOpenALError())
                    {
                        Log(Log::Level::ERR) << "Failed to delete OpenAL buffer";
                    }
                }
            }

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

            format40 = alGetEnumValue("AL_FORMAT_QUAD16");
            format51 = alGetEnumValue("AL_FORMAT_51CHN16");
            format61 = alGetEnumValue("AL_FORMAT_61CHN16");
            format71 = alGetEnumValue("AL_FORMAT_71CHN16");

            alGenSources(1, &sourceId);

            if (AudioAL::checkOpenALError())
            {
                Log(Log::Level::ERR) << "Failed to create OpenAL source";
                return false;
            }

            alGenBuffers(2, buffers);

            if (AudioAL::checkOpenALError())
            {
                Log(Log::Level::ERR) << "Failed to create OpenAL buffers";
                return false;
            }

            switch (channels)
            {
                case 1: format = AL_FORMAT_MONO16; break;
                case 2: format = AL_FORMAT_STEREO16; break;
                case 4: format = format40; break;
                case 6: format = format51; break;
                case 7: format = format61; break;
                case 8: format = format71; break;
                default:
                {
                    Log(Log::Level::ERR) << "Invalid channel count";
                    return false;
                }
            }

            std::vector<uint8_t> data1 = getData(bufferSize);

            alBufferData(buffers[0], format,
                         data1.data(),
                         static_cast<ALsizei>(data1.size()),
                         static_cast<ALsizei>(samplesPerSecond));

            std::vector<uint8_t> data2 = getData(bufferSize);

            alBufferData(buffers[1], format,
                         data2.data(),
                         static_cast<ALsizei>(data2.size()),
                         static_cast<ALsizei>(samplesPerSecond));

            nextBuffer = 0;

            alSourceQueueBuffers(sourceId, 2, buffers);

            if (AudioAL::checkOpenALError())
            {
                Log(Log::Level::ERR) << "Failed to queue OpenAL buffers";
                return false;
            }

            alSourcePlay(sourceId);

            if (AudioAL::checkOpenALError())
            {
                Log(Log::Level::ERR) << "Failed to play OpenAL source";
                return false;
            }

            return Audio::init();
        }

        bool AudioAL::update()
        {
            Audio::update();

            alcMakeContextCurrent(context);

            if (checkALCError())
            {
                Log(Log::Level::ERR) << "Failed to make OpenAL context current";
                return false;
            }

            ALint buffersProcessed;
            alGetSourcei(sourceId, AL_BUFFERS_PROCESSED, &buffersProcessed);

            if (AudioAL::checkOpenALError())
            {
                Log(Log::Level::ERR) << "Failed to get processed buffer count";
                return false;
            }

            // requeue all processed buffers
            for (; buffersProcessed > 0; --buffersProcessed)
            {
                alSourceUnqueueBuffers(sourceId, 1, &buffers[nextBuffer]);

                if (AudioAL::checkOpenALError())
                {
                    Log(Log::Level::ERR) << "Failed to unqueue OpenAL buffer";
                    return false;
                }

                std::vector<uint8_t> data = getData(bufferSize);

                if (!data.empty())
                {
                    alBufferData(buffers[nextBuffer], format,
                                 data.data(),
                                 static_cast<ALsizei>(data.size()),
                                 static_cast<ALsizei>(samplesPerSecond));

                    alSourceQueueBuffers(sourceId, 1, &buffers[nextBuffer]);

                    if (AudioAL::checkOpenALError())
                    {
                        Log(Log::Level::ERR) << "Failed to queue OpenAL buffer";
                        return false;
                    }

                    ALint state;
                    alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
                    if (state != AL_PLAYING)
                    {
                        alSourcePlay(sourceId);

                        if (AudioAL::checkOpenALError())
                        {
                            Log(Log::Level::ERR) << "Failed to play OpenAL source";
                            return false;
                        }
                    }
                }

                // swap the buffer
                nextBuffer = (nextBuffer == 0) ? 1 : 0;
            }

            std::lock_guard<std::mutex> lock(uploadMutex);
            dirty = 0;

            return true;
        }

        SoundResource* AudioAL::createSound()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            SoundResource* sound = new SoundResource();
            resources.push_back(std::unique_ptr<SoundResource>(sound));
            return sound;
        }
    } // namespace audio
} // namespace ouzel

#endif
