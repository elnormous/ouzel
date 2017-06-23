// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENAL

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

                    AudioAL* audioAL = static_cast<AudioAL*>(sharedEngine->getAudio());

                    ALenum format = audioAL->getFormatForChannels(soundData->getChannels());

                    if (format == 0)
                    {
                        Log(Log::Level::ERR) << "Unsupported audio format";
                        return false;
                    }

                    const std::vector<uint8_t>& data = soundData->getData();

                    alBufferData(outputBuffer, format,
                                 data.data(),
                                 static_cast<ALsizei>(data.size()),
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

#endif
