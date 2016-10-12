// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundSL.h"
#include "AudioSL.h"
#include "SoundDataSL.h"
#include "core/Engine.h"
#include "utils/Log.h"

static void playerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void* context)
{
    ouzel::audio::SoundSL* sound = reinterpret_cast<ouzel::audio::SoundSL*>(context);

    if ((*bufferQueue)->Enqueue(bufferQueue, sound->getSoundData()->getData().data(), sound->getSoundData()->getData().size()) != SL_RESULT_SUCCESS)
    {
        ouzel::Log(ouzel::Log::Level::ERR) << "Failed to enqueue OpenSL data";
    }

    if (!sound->isRepeating())
    {
        SLPlayItf player = sound->getPlayer();

        if ((*player)->SetPlayState(player, SL_PLAYSTATE_STOPPED) != SL_RESULT_SUCCESS)
        {
            ouzel::Log(ouzel::Log::Level::ERR) << "Failed to stop sound";
        }
    }
}

namespace ouzel
{
    namespace audio
    {
        SoundSL::SoundSL()
        {
        }

        SoundSL::~SoundSL()
        {
            if (playerObject)
            {
                (*playerObject)->Destroy(playerObject);
            }
        }

        void SoundSL::free()
        {
            Sound::free();

            if (playerObject)
            {
                (*playerObject)->Destroy(playerObject);
                playerObject = nullptr;
                player = nullptr;
                bufferQueue = nullptr;
                playerVolume = nullptr;
            }
        }

        bool SoundSL::init(const SoundDataPtr& newSoundData)
        {
            if (!Sound::init(newSoundData))
            {
                return false;
            }

            free();

            std::shared_ptr<AudioSL> audioSL = std::static_pointer_cast<AudioSL>(sharedEngine->getAudio());

            SLDataLocator_AndroidSimpleBufferQueue location = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };

            SLDataFormat_PCM format;
            format.formatType = SL_DATAFORMAT_PCM;
            format.numChannels = soundData->getChannels();
            format.samplesPerSec = soundData->getSamplesPerSecond() * 1000; //mHz
            format.bitsPerSample = soundData->getBitsPerSample();
            format.containerSize = soundData->getBitsPerSample();

            if (soundData->getChannels() == 2)
            {
                format.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
            }
            else if (soundData->getChannels() == 1)
            {
                format.channelMask = SL_SPEAKER_FRONT_CENTER;
            }
            else
            {
                Log(Log::Level::ERR) << "Invalid channel count";
                return false;
            }

            format.endianness = SL_BYTEORDER_LITTLEENDIAN;

            SLDataSource dataSource = { &location, &format };

            SLDataLocator_OutputMix dataLocatorOut;
            dataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX;
            dataLocatorOut.outputMix = audioSL->getOutputMix();

            SLDataSink dataSink;
            dataSink.pLocator = &dataLocatorOut;
            dataSink.pFormat = NULL;

            const SLuint32 playerIIDCount = 3;
            const SLInterfaceID playerIIDs[] = { SL_IID_BUFFERQUEUE, SL_IID_PLAY, SL_IID_VOLUME };
            const SLboolean playerReqs[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

            if ((*audioSL->getEngine())->CreateAudioPlayer(audioSL->getEngine(), &playerObject, &dataSource, &dataSink, playerIIDCount, playerIIDs, playerReqs) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to create OpenSL player object";
                return false;
            }

            if ((*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to create OpenSL player object";
                return false;
            }

            if ((*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &player) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to get OpenSL player";
                return false;
            }

            if ((*playerObject)->GetInterface(playerObject, SL_IID_BUFFERQUEUE, &bufferQueue) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to get OpenSL buffer queue";
                return false;
            }

            if ((*playerObject)->GetInterface(playerObject, SL_IID_VOLUME, &playerVolume) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to get OpenSL volume";
                return false;
            }

            if ((*bufferQueue)->Clear(bufferQueue) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to clear OpenSL buffer";
                return false;
            }

            if ((*bufferQueue)->RegisterCallback(bufferQueue, playerCallback, this) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to register OpenSL buffer queue callback";
                return false;
            }

            if ((*bufferQueue)->Enqueue(bufferQueue, soundData->getData().data(), soundData->getData().size()) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to enqueue OpenSL data";
                return false;
            }

            ready = true;

            return true;
        }

        bool SoundSL::play(bool repeatSound)
        {
            if (!Sound::play(repeatSound))
            {
                return false;
            }

            if ((*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to play sound";
                return false;
            }

            return true;
        }

        bool SoundSL::stop(bool resetSound)
        {
            if (!Sound::stop(resetSound))
            {
                return false;
            }

            if ((*player)->SetPlayState(player, resetSound ? SL_PLAYSTATE_STOPPED : SL_PLAYSTATE_PAUSED) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to " << (resetSound ? "stop" : "pause") << " sound";
                return false;
            }

            return true;
        }

        bool SoundSL::reset()
        {
            if (!Sound::reset())
            {
                return false;
            }

            if ((*player)->SetPlayState(player, SL_PLAYSTATE_STOPPED) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to stop sound";
                return false;
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
