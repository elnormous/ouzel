// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundAL.h"
#include "AudioAL.h"
#include "audio/SoundData.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace audio
    {
        SoundAL::SoundAL()
        {
        }

        SoundAL::~SoundAL()
        {
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

            if (outputBuffer)
            {
                alDeleteBuffers(1, &outputBuffer);

                if (AudioAL::checkOpenALError())
                {
                    Log(Log::Level::ERR) << "Failed to delete OpenAL buffer";
                }
            }
        }

        bool SoundAL::update()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty & DIRTY_SOUND_DATA)
            {
                if (soundData)
                {
                    if (!sourceId) alGenSources(1, &sourceId);

                    if (AudioAL::checkOpenALError())
                    {
                        Log(Log::Level::ERR) << "Failed to create OpenAL source";
                        return false;
                    }

                    if (!outputBuffer) alGenBuffers(1, &outputBuffer);

                    if (AudioAL::checkOpenALError())
                    {
                        Log(Log::Level::ERR) << "Failed to create OpenAL buffer";
                        return false;
                    }

                    ALenum format = 0;

                    if (soundData->getChannels() == 1)
                    {
                        format = AL_FORMAT_MONO16;
                    }
                    else if (soundData->getChannels() == 2)
                    {
                        format = AL_FORMAT_STEREO16;
                    }
                    else if (soundData->getChannels() == 4)
                    {
                        format = alGetEnumValue("AL_FORMAT_QUAD16");
                    }
                    else if (soundData->getChannels() == 6)
                    {
                        format = alGetEnumValue("AL_FORMAT_51CHN16");
                    }
                    else if (soundData->getChannels() == 7)
                    {
                        format = alGetEnumValue("AL_FORMAT_61CHN16");
                    }
                    else if (soundData->getChannels() == 8)
                    {
                        format = alGetEnumValue("AL_FORMAT_71CHN16");
                    }

                    if (format == 0)
                    {
                        Log(Log::Level::ERR) << "Unsupported audio format";
                        return false;
                    }

                    std::vector<uint8_t> buffer;
                    soundData->getData(buffer);

                    alBufferData(outputBuffer, format,
                                 buffer.data(),
                                 static_cast<ALsizei>(buffer.size()),
                                 static_cast<ALsizei>(soundData->getSamplesPerSecond()));

                    if (AudioAL::checkOpenALError())
                    {
                        Log(Log::Level::ERR) << "Failed to upload OpenAL data";
                        return false;
                    }

                    alSourcei(sourceId, AL_BUFFER, static_cast<ALint>(outputBuffer));

                    if (AudioAL::checkOpenALError())
                    {
                        Log(Log::Level::ERR) << "Failed to set OpenAL buffer";
                        return false;
                    }
                }
            }

            if (dirty & DIRTY_PITCH)
            {
                if (sourceId)
                {
                    alSourcef(sourceId, AL_PITCH, pitch);
                }
            }

            if (dirty & DIRTY_GAIN)
            {
                if (sourceId)
                {
                    alSourcef(sourceId, AL_GAIN, gain);
                }
            }

            if (dirty & DIRTY_POSITION)
            {
                if (sourceId)
                {
                    alSourcefv(sourceId, AL_POSITION, position.v);
                }
            }

            if (dirty & DIRTY_PLAY_STATE)
            {
                if (sourceId)
                {
                    if (shouldPlay)
                    {
                        if (reset)
                        {
                            alSourceRewind(sourceId);
                        }

                        alSourcei(sourceId, AL_LOOPING, repeat ? AL_TRUE : AL_FALSE);
                        alSourcePlay(sourceId);

                        if (AudioAL::checkOpenALError())
                        {
                            Log(Log::Level::ERR) << "Failed to play OpenAL source";
                            return false;
                        }
                    }
                    else
                    {
                        if (reset)
                        {
                            alSourceStop(sourceId);

                            if (AudioAL::checkOpenALError())
                            {
                                Log(Log::Level::ERR) << "Failed to stop OpenAL source";
                                return false;
                            }
                        }
                        else
                        {
                            alSourcePause(sourceId);

                            if (AudioAL::checkOpenALError())
                            {
                                Log(Log::Level::ERR) << "Failed to pause OpenAL source";
                                return false;
                            }
                        }
                    }
                }

                reset = false;
            }
            
            dirty = 0;

            return true;
        }
    } // namespace audio
} // namespace ouzel
