// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENAL

#include "SoundResourceAL.h"
#include "AudioAL.h"
#include "audio/SoundData.h"
#include "audio/Stream.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace audio
    {
        static const uint32_t BUFFER_SIZE = 32 * 4096;

        SoundResourceAL::SoundResourceAL()
        {
            std::fill(std::begin(buffers), std::end(buffers), 0);
        }

        SoundResourceAL::~SoundResourceAL()
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
        }

        bool SoundResourceAL::update()
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

                    for (ALuint buffer : buffers)
                    {
                        if (buffer)
                        {
                            alDeleteBuffers(1, &buffer);

                            if (AudioAL::checkOpenALError())
                            {
                                Log(Log::Level::ERR) << "Failed to delete OpenAL buffer";
                                return false;
                            }
                        }
                    }

                    AudioAL* audioAL = static_cast<AudioAL*>(sharedEngine->getAudio());

                    format = audioAL->getFormatForChannels(soundData->getChannels());

                    if (format == 0)
                    {
                        Log(Log::Level::ERR) << "Unsupported audio format";
                        return false;
                    }

                    if (stream)
                    {
                        alGenBuffers(2, buffers);

                        if (AudioAL::checkOpenALError())
                        {
                            Log(Log::Level::ERR) << "Failed to create OpenAL buffers";
                            return false;
                        }

                        std::vector<uint8_t> data = soundData->getData(stream.get(), BUFFER_SIZE);

                        alBufferData(buffers[0], format,
                                     data.data(),
                                     static_cast<ALsizei>(data.size()),
                                     static_cast<ALsizei>(soundData->getSamplesPerSecond()));

                        data = soundData->getData(stream.get(), BUFFER_SIZE);

                        alBufferData(buffers[1], format,
                                     data.data(),
                                     static_cast<ALsizei>(data.size()),
                                     static_cast<ALsizei>(soundData->getSamplesPerSecond()));

                        nextBuffer = 0;

                        alSourceQueueBuffers(sourceId, 2, buffers);

                        if (AudioAL::checkOpenALError())
                        {
                            Log(Log::Level::ERR) << "Failed to queue OpenAL buffers";
                            return false;
                        }
                    }
                    else
                    {
                        alGenBuffers(1, buffers);

                        if (AudioAL::checkOpenALError())
                        {
                            Log(Log::Level::ERR) << "Failed to create OpenAL buffer";
                            return false;
                        }

                        const std::vector<uint8_t>& data = soundData->getData();

                        alBufferData(buffers[0], format,
                                     data.data(),
                                     static_cast<ALsizei>(data.size()),
                                     static_cast<ALsizei>(soundData->getSamplesPerSecond()));

                        if (AudioAL::checkOpenALError())
                        {
                            Log(Log::Level::ERR) << "Failed to upload OpenAL data";
                            return false;
                        }

                        alSourcei(sourceId, AL_BUFFER, static_cast<ALint>(buffers[0]));

                        if (AudioAL::checkOpenALError())
                        {
                            Log(Log::Level::ERR) << "Failed to set OpenAL buffer";
                            return false;
                        }
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

                            if (stream)
                            {
                                stream->reset();
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

            if (shouldPlay && soundData && stream && sourceId)
            {
                ALint buffersProcessed;
                alGetSourcei(sourceId, AL_BUFFERS_PROCESSED, &buffersProcessed);

                // requeue all processed buffers
                for (; buffersProcessed > 0; --buffersProcessed)
                {
                    alSourceUnqueueBuffers(sourceId, 1, &buffers[nextBuffer]);

                    if (AudioAL::checkOpenALError())
                    {
                        Log(Log::Level::ERR) << "Failed to unqueue OpenAL buffer";
                        return false;
                    }

                    std::vector<uint8_t> data = soundData->getData(stream.get(), BUFFER_SIZE);

                    // if stream has ended but we should repeat it
                    if (repeat && data.empty())
                    {
                        stream->reset();
                        data = soundData->getData(stream.get(), BUFFER_SIZE);
                    }

                    if (!data.empty())
                    {
                        alBufferData(buffers[nextBuffer], format,
                                     data.data(),
                                     static_cast<ALsizei>(data.size()),
                                     static_cast<ALsizei>(soundData->getSamplesPerSecond()));

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
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel

#endif
