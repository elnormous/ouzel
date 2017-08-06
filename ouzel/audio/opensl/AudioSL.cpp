// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENSL

#include "AudioSL.h"
#include "utils/Log.h"

static void playerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void* context)
{
    ouzel::audio::AudioSL* audioSL = reinterpret_cast<ouzel::audio::AudioSL*>(context);

    audioSL->enqueue(bufferQueue);
}

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
            if (playerObject)
            {
                (*playerObject)->Destroy(playerObject);
            }

            if (outputMixObject)
            {
                (*outputMixObject)->Destroy(outputMixObject);
            }

            if (engineObject)
            {
                (*engineObject)->Destroy(engineObject);
            }
        }

        bool AudioSL::init(bool debugAudio)
        {
            if (!Audio::init(debugAudio))
            {
                return false;
            }

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

            SLDataLocator_AndroidSimpleBufferQueue location = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};

            SLDataFormat_PCM format;
            format.formatType = SL_DATAFORMAT_PCM;
            format.numChannels = channels;
            format.samplesPerSec = sampleRate * 1000; //mHz
            format.bitsPerSample = 16;
            format.containerSize = format.bitsPerSample;

            switch (channels)
            {
                case 1: format.channelMask = SL_SPEAKER_FRONT_CENTER; break;
                case 2: format.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT; break;
                case 4: format.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT | SL_SPEAKER_BACK_LEFT | SL_SPEAKER_BACK_RIGHT; break;
                case 6: format.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT | SL_SPEAKER_FRONT_CENTER | SL_SPEAKER_LOW_FREQUENCY | SL_SPEAKER_SIDE_LEFT|SL_SPEAKER_SIDE_RIGHT; break;
                case 7: format.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT | SL_SPEAKER_FRONT_CENTER | SL_SPEAKER_LOW_FREQUENCY | SL_SPEAKER_BACK_CENTER | SL_SPEAKER_SIDE_LEFT | SL_SPEAKER_SIDE_RIGHT; break;
                case 8: format.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT | SL_SPEAKER_FRONT_CENTER | SL_SPEAKER_LOW_FREQUENCY | SL_SPEAKER_BACK_LEFT|SL_SPEAKER_BACK_RIGHT | SL_SPEAKER_SIDE_LEFT | SL_SPEAKER_SIDE_RIGHT; break;
                default:
                    Log(Log::Level::ERR) << "Invalid channel count";
                    return false;
            }

            format.endianness = SL_BYTEORDER_LITTLEENDIAN;

            SLDataSource dataSource = {&location, &format};

            SLDataLocator_OutputMix dataLocatorOut;
            dataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX;
            dataLocatorOut.outputMix = outputMixObject;

            SLDataSink dataSink;
            dataSink.pLocator = &dataLocatorOut;
            dataSink.pFormat = NULL;

            const SLuint32 playerIIDCount = 3;
            const SLInterfaceID playerIIDs[] = {SL_IID_BUFFERQUEUE, SL_IID_PLAY, SL_IID_VOLUME};
            const SLboolean playerReqs[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

            if ((*engine)->CreateAudioPlayer(engine, &playerObject, &dataSource, &dataSink, playerIIDCount, playerIIDs, playerReqs) != SL_RESULT_SUCCESS)
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
                Log(Log::Level::ERR) << "Failed to get OpenSL player interface";
                return false;
            }

            if ((*playerObject)->GetInterface(playerObject, SL_IID_BUFFERQUEUE, &bufferQueue) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to get OpenSL buffer queue interface";
                return false;
            }

            if ((*playerObject)->GetInterface(playerObject, SL_IID_VOLUME, &playerVolume) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to get OpenSL volume interface";
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
            
            if (!getData(bufferSize / sizeof(int16_t), Format::SINT16, data))
            {
                return false;
            }

            if ((*bufferQueue)->Enqueue(bufferQueue, data.data(), data.size()) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to enqueue OpenSL data";
                return false;
            }

            if ((*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING) != SL_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to play sound";
                return false;
            }

            return true;
        }

        void AudioSL::enqueue(SLAndroidSimpleBufferQueueItf bufferQueue)
        {
            if (!update())
            {
                return;
            }

            if (!getData(bufferSize / sizeof(int16_t), Format::SINT16, data))
            {
                return;
            }

            if ((*bufferQueue)->Enqueue(bufferQueue, data.data(), data.size()) != SL_RESULT_SUCCESS)
            {
                ouzel::Log(ouzel::Log::Level::ERR) << "Failed to enqueue OpenSL data";
            }
        }
    } // namespace audio
} // namespace ouzel

#endif
