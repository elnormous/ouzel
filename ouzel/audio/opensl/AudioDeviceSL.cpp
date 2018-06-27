// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENSL

#include "AudioDeviceSL.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

static void playerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void* context)
{
    ouzel::audio::AudioDeviceSL* audioDeviceSL = reinterpret_cast<ouzel::audio::AudioDeviceSL*>(context);

    audioDeviceSL->enqueue(bufferQueue);
}

namespace ouzel
{
    namespace audio
    {
        AudioDeviceSL::AudioDeviceSL():
            AudioDevice(Audio::Driver::OPENSL)
        {
        }

        AudioDeviceSL::~AudioDeviceSL()
        {
            if (playerObject)
                (*playerObject)->Destroy(playerObject);

            if (outputMixObject)
                (*outputMixObject)->Destroy(outputMixObject);

            if (engineObject)
                (*engineObject)->Destroy(engineObject);
        }

        void AudioDeviceSL::init(bool debugAudio)
        {
            AudioDevice::init(debugAudio);

            const SLuint32 engineMixIIDCount = 1;
            const SLInterfaceID engineMixIID = SL_IID_ENGINE;
            const SLboolean engineMixReq = SL_BOOLEAN_TRUE;

            if (slCreateEngine(&engineObject, 0, nullptr, engineMixIIDCount, &engineMixIID, &engineMixReq) != SL_RESULT_SUCCESS)
                throw SystemError("Failed to create OpenSL engine object");

            if ((*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
                throw SystemError("Failed to create OpenSL engine object");

            if ((*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engine) != SL_RESULT_SUCCESS)
                throw SystemError("Failed to get OpenSL engine");

            if ((*engine)->CreateOutputMix(engine, &outputMixObject, 0, nullptr, nullptr) != SL_RESULT_SUCCESS)
                throw SystemError("Failed to create OpenSL output mix");

            if ((*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
                throw SystemError("Failed to create OpenSL output mix");

            SLDataLocator_AndroidSimpleBufferQueue location = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};

            SLDataFormat_PCM dataFormat;
            dataFormat.formatType = SL_DATAFORMAT_PCM;
            dataFormat.numChannels = channels;
            dataFormat.samplesPerSec = sampleRate * 1000; //mHz
            dataFormat.bitsPerSample = 16;
            dataFormat.containerSize = dataFormat.bitsPerSample;

            switch (channels)
            {
                case 1: dataFormat.channelMask = SL_SPEAKER_FRONT_CENTER; break;
                case 2: dataFormat.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT; break;
                case 4: dataFormat.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT | SL_SPEAKER_BACK_LEFT | SL_SPEAKER_BACK_RIGHT; break;
                case 6: dataFormat.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT | SL_SPEAKER_FRONT_CENTER | SL_SPEAKER_LOW_FREQUENCY | SL_SPEAKER_SIDE_LEFT|SL_SPEAKER_SIDE_RIGHT; break;
                case 7: dataFormat.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT | SL_SPEAKER_FRONT_CENTER | SL_SPEAKER_LOW_FREQUENCY | SL_SPEAKER_BACK_CENTER | SL_SPEAKER_SIDE_LEFT | SL_SPEAKER_SIDE_RIGHT; break;
                case 8: dataFormat.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT | SL_SPEAKER_FRONT_CENTER | SL_SPEAKER_LOW_FREQUENCY | SL_SPEAKER_BACK_LEFT|SL_SPEAKER_BACK_RIGHT | SL_SPEAKER_SIDE_LEFT | SL_SPEAKER_SIDE_RIGHT; break;
                default:
                    throw SystemError("Invalid channel count");
            }

            dataFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;

            SLDataSource dataSource = {&location, &dataFormat};

            SLDataLocator_OutputMix dataLocatorOut;
            dataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX;
            dataLocatorOut.outputMix = outputMixObject;

            SLDataSink dataSink;
            dataSink.pLocator = &dataLocatorOut;
            dataSink.pFormat = nullptr;

            const SLuint32 playerIIDCount = 3;
            const SLInterfaceID playerIIDs[] = {SL_IID_BUFFERQUEUE, SL_IID_PLAY, SL_IID_VOLUME};
            const SLboolean playerReqs[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

            if ((*engine)->CreateAudioPlayer(engine, &playerObject, &dataSource, &dataSink, playerIIDCount, playerIIDs, playerReqs) != SL_RESULT_SUCCESS)
                throw SystemError("Failed to create OpenSL player object");

            sampleFormat = Audio::SampleFormat::SINT16;

            if ((*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
                throw SystemError("Failed to create OpenSL player object");

            if ((*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &player) != SL_RESULT_SUCCESS)
                throw SystemError("Failed to get OpenSL player interface");

            if ((*playerObject)->GetInterface(playerObject, SL_IID_BUFFERQUEUE, &bufferQueue) != SL_RESULT_SUCCESS)
                throw SystemError("Failed to get OpenSL buffer queue interface");

            if ((*playerObject)->GetInterface(playerObject, SL_IID_VOLUME, &playerVolume) != SL_RESULT_SUCCESS)
                throw SystemError("Failed to get OpenSL volume interface");

            if ((*bufferQueue)->Clear(bufferQueue) != SL_RESULT_SUCCESS)
                throw SystemError("Failed to clear OpenSL buffer");

            if ((*bufferQueue)->RegisterCallback(bufferQueue, playerCallback, this) != SL_RESULT_SUCCESS)
                throw SystemError("Failed to register OpenSL buffer queue callback");

            getData(bufferSize / (channels * sizeof(int16_t)), data);

            if ((*bufferQueue)->Enqueue(bufferQueue, data.data(), data.size()) != SL_RESULT_SUCCESS)
                throw SystemError("Failed to enqueue OpenSL data");

            if ((*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING) != SL_RESULT_SUCCESS)
                throw SystemError("Failed to play sound");
        }

        void AudioDeviceSL::enqueue(SLAndroidSimpleBufferQueueItf bufferQueue)
        {
            process();

            getData(bufferSize / (channels * sizeof(int16_t)), data);

            if ((*bufferQueue)->Enqueue(bufferQueue, data.data(), data.size()) != SL_RESULT_SUCCESS)
                throw SystemError("Failed to enqueue OpenSL data");
        }
    } // namespace audio
} // namespace ouzel

#endif
