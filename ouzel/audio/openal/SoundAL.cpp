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

        bool SoundAL::init(const std::shared_ptr<SoundData>& newSoundData)
        {
            if (!Sound::init(newSoundData))
            {
                return false;
            }

            alGenSources(1, &sourceId);

            if (AudioAL::checkOpenALError())
            {
                Log(Log::Level::ERR) << "Failed to create OpenAL source";
                return false;
            }

            alGenBuffers(1, &outputBuffer);

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

            alSourcef(sourceId, AL_PITCH, 1.0f);
            alSourcef(sourceId, AL_GAIN, 1.0f);

            alSourcei(sourceId, AL_BUFFER, static_cast<ALint>(outputBuffer));

            if (AudioAL::checkOpenALError())
            {
                Log(Log::Level::ERR) << "Failed to set OpenAL buffer";
                return false;
            }

            ready = true;

            return true;
        }

        void SoundAL::setPosition(const Vector3& newPosition)
        {
            Sound::setPosition(newPosition);
            alSourcefv(sourceId, AL_POSITION, position.v);
        }

        void SoundAL::setPitch(float newPitch)
        {
            Sound::setPitch(newPitch);
            alSourcef(sourceId, AL_PITCH, pitch);
        }

        void SoundAL::setGain(float newGain)
        {
            Sound::setGain(newGain);
            alSourcef(sourceId, AL_GAIN, gain);
        }

        bool SoundAL::play(bool repeatSound)
        {
            if (!Sound::play(repeatSound))
            {
                return false;
            }

            alSourcei(sourceId, AL_LOOPING, repeatSound ? AL_TRUE : AL_FALSE);
            alSourcePlay(sourceId);

            if (AudioAL::checkOpenALError())
            {
                Log(Log::Level::ERR) << "Failed to play OpenAL source";
                return false;
            }

            return true;
        }

        bool SoundAL::stop(bool resetSound)
        {
            if (!Sound::stop(resetSound))
            {
                return false;
            }

            if (resetSound)
            {
                alSourceStop(sourceId);
            }
            else
            {
                alSourcePause(sourceId);
            }

            if (AudioAL::checkOpenALError())
            {
                Log(Log::Level::ERR) << "Failed to stop OpenAL source";
                return false;
            }

            return true;
        }

        bool SoundAL::reset()
        {
            if (!Sound::reset())
            {
                return false;
            }

            alSourceRewind(sourceId);

            if (AudioAL::checkOpenALError())
            {
                Log(Log::Level::ERR) << "Failed to rewind OpenAL source";
                return false;
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
