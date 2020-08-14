// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENSL

#include <system_error>
#include "OSLAudioDevice.hpp"
#include "OSLErrorCategory.hpp"
#include "../../core/Engine.hpp"
#include "../../utils/Log.hpp"

namespace ouzel::audio::opensl
{
    namespace
    {
        const ErrorCategory errorCategory{};

        std::error_code makeErrorCode(SLresult e)
        {
            return std::error_code(static_cast<int>(e), errorCategory);
        }

        void playerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void* context)
        {
            try
            {
                auto audioDevice = static_cast<AudioDevice*>(context);

                audioDevice->enqueue(bufferQueue);
            }
            catch (const std::exception& e)
            {
                logger.log(Log::Level::error) << e.what();
            }
        }
    }

    AudioDevice::AudioDevice(const Settings& settings,
                             const std::function<void(std::uint32_t frames,
                                                      std::uint32_t channels,
                                                      std::uint32_t sampleRate,
                                                      std::vector<float>& samples)>& initDataGetter):
        audio::AudioDevice(Driver::openSL, settings, initDataGetter)
    {
        SLresult result;
        constexpr SLuint32 engineMixIIDCount = 1;
        const auto engineMixIID = SL_IID_ENGINE;
        constexpr SLboolean engineMixReq = SL_BOOLEAN_TRUE;

        SLObjectItf engineObjectPointer;
        if ((result = slCreateEngine(&engineObjectPointer, 0, nullptr, engineMixIIDCount, &engineMixIID, &engineMixReq)) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to create OpenSL engine object");
        
        engineObject = engineObjectPointer;

        if ((result = engineObject->Realize(engineObject.get(), SL_BOOLEAN_FALSE)) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to create OpenSL engine object");

        if ((result = engineObject->GetInterface(engineObject.get(), SL_IID_ENGINE, &engine)) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to get OpenSL engine");

        SLObjectItf outputMixObjectPointer;
        if ((result = (*engine)->CreateOutputMix(engine, &outputMixObjectPointer, 0, nullptr, nullptr)) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to create OpenSL output mix");
        
        outputMixObject = outputMixObjectPointer;

        if ((result = outputMixObject->Realize(outputMixObject.get(), SL_BOOLEAN_FALSE)) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to create OpenSL output mix");

        SLDataLocator_AndroidSimpleBufferQueue location = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};

        SLDataFormat_PCM dataFormat;
        dataFormat.formatType = SL_DATAFORMAT_PCM;
        // TODO: get speaker count
        dataFormat.numChannels = channels;
        dataFormat.samplesPerSec = sampleRate * 1000; // mHz
        dataFormat.bitsPerSample = sizeof(std::int16_t) * 8;
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
                throw std::runtime_error("Invalid channel count");
        }

        dataFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;

        SLDataSource dataSource{&location, &dataFormat};
        SLDataLocator_OutputMix dataLocatorOut{SL_DATALOCATOR_OUTPUTMIX, outputMixObject.get()};
        SLDataSink dataSink{&dataLocatorOut, nullptr};
        constexpr SLuint32 playerIIDCount = 3;
        const SLInterfaceID playerIIDs[] = {SL_IID_BUFFERQUEUE, SL_IID_PLAY, SL_IID_VOLUME};
        constexpr SLboolean playerReqs[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

        SLObjectItf playerObjectPointer;
        if ((result = (*engine)->CreateAudioPlayer(engine, &playerObjectPointer, &dataSource, &dataSink, playerIIDCount, playerIIDs, playerReqs)) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to create OpenSL player object");

        playerObject = playerObjectPointer;

        sampleFormat = SampleFormat::signedInt16;

        if ((result = playerObject->Realize(playerObject.get(), SL_BOOLEAN_FALSE)) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to create OpenSL player object");

        if ((result = playerObject->GetInterface(playerObject.get(), SL_IID_PLAY, &player)) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to get OpenSL player interface");

        if ((result = playerObject->GetInterface(playerObject.get(), SL_IID_BUFFERQUEUE, &bufferQueue)) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to get OpenSL buffer queue interface");

        if ((result = playerObject->GetInterface(playerObject.get(), SL_IID_VOLUME, &playerVolume)) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to get OpenSL volume interface");

        if ((result = (*bufferQueue)->Clear(bufferQueue)) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to clear OpenSL buffer");

        if ((result = (*bufferQueue)->RegisterCallback(bufferQueue, playerCallback, this)) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to register OpenSL buffer queue callback");
    }

    void AudioDevice::start()
    {
        getData(bufferSize / (channels * sizeof(std::int16_t)), data);

        SLresult result;
        if ((result = (*bufferQueue)->Enqueue(bufferQueue, data.data(), data.size())) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to enqueue OpenSL data");

        if ((result = (*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING)) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to play sound");
    }

    void AudioDevice::stop()
    {
        SLresult result;
        if ((result = (*player)->SetPlayState(player, SL_PLAYSTATE_STOPPED)) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to stop sound");
    }

    void AudioDevice::enqueue(SLAndroidSimpleBufferQueueItf bufferQueue)
    {
        getData(bufferSize / (channels * sizeof(std::int16_t)), data);

        SLresult result;
        if ((result = (*bufferQueue)->Enqueue(bufferQueue, data.data(), data.size())) != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to enqueue OpenSL data");
    }
}
#endif
