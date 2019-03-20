// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENSL

#include <system_error>
#include "OSLAudioDevice.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"

static void playerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void* context)
{
    try
    {
        ouzel::audio::OSLAudioDevice* audioDeviceSL = reinterpret_cast<ouzel::audio::OSLAudioDevice*>(context);

        audioDeviceSL->enqueue(bufferQueue);
    }
    catch (const std::exception& e)
    {
        ouzel::engine->log(ouzel::Log::Level::ERR) << e.what();
    }
}

namespace ouzel
{
    namespace audio
    {
        class OpenSLErrorCategory final: public std::error_category
        {
        public:
            const char* name() const noexcept override
            {
                return "OpenSL";
            }

            std::string message(int condition) const override
            {
                switch (condition)
                {
                    case SL_RESULT_PRECONDITIONS_VIOLATED: return "SL_RESULT_PRECONDITIONS_VIOLATED";
                    case SL_RESULT_PARAMETER_INVALID: return "SL_RESULT_PARAMETER_INVALID";
                    case SL_RESULT_MEMORY_FAILURE: return "SL_RESULT_MEMORY_FAILURE";
                    case SL_RESULT_RESOURCE_ERROR: return "SL_RESULT_RESOURCE_ERROR";
                    case SL_RESULT_RESOURCE_LOST: return "SL_RESULT_RESOURCE_LOST";
                    case SL_RESULT_IO_ERROR: return "SL_RESULT_IO_ERROR";
                    case SL_RESULT_BUFFER_INSUFFICIENT: return "SL_RESULT_BUFFER_INSUFFICIENT";
                    case SL_RESULT_CONTENT_CORRUPTED: return "SL_RESULT_CONTENT_CORRUPTED";
                    case SL_RESULT_CONTENT_UNSUPPORTED: return "SL_RESULT_CONTENT_UNSUPPORTED";
                    case SL_RESULT_CONTENT_NOT_FOUND: return "SL_RESULT_CONTENT_NOT_FOUND";
                    case SL_RESULT_PERMISSION_DENIED: return "SL_RESULT_PERMISSION_DENIED";
                    case SL_RESULT_FEATURE_UNSUPPORTED: return "SL_RESULT_FEATURE_UNSUPPORTED";
                    case SL_RESULT_INTERNAL_ERROR: return "SL_RESULT_INTERNAL_ERROR";
                    case SL_RESULT_UNKNOWN_ERROR: return "SL_RESULT_UNKNOWN_ERROR";
                    case SL_RESULT_OPERATION_ABORTED: return "SL_RESULT_OPERATION_ABORTED";
                    case SL_RESULT_CONTROL_LOST: return "SL_RESULT_CONTROL_LOST";
                    default: return "Unknown error (" + std::to_string(condition) + ")";
                }
            }
        };

        const OpenSLErrorCategory openSLErrorCategory {};

        static std::error_code makeErrorCode(SLresult e)
        {
            return std::error_code(static_cast<int>(e), openSLErrorCategory);
        }

        OSLAudioDevice::OSLAudioDevice(const std::function<void(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)>& initDataGetter):
            AudioDevice(Driver::OPENSL, initDataGetter)
        {
            SLresult result;
            constexpr SLuint32 engineMixIIDCount = 1;
            const SLInterfaceID engineMixIID = SL_IID_ENGINE;
            constexpr SLboolean engineMixReq = SL_BOOLEAN_TRUE;

            if ((result = slCreateEngine(&engineObject, 0, nullptr, engineMixIIDCount, &engineMixIID, &engineMixReq)) != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to create OpenSL engine object");

            if ((result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE)) != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to create OpenSL engine object");

            if ((result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engine)) != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to get OpenSL engine");

            if ((result = (*engine)->CreateOutputMix(engine, &outputMixObject, 0, nullptr, nullptr)) != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to create OpenSL output mix");

            if ((result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE)) != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to create OpenSL output mix");

            SLDataLocator_AndroidSimpleBufferQueue location = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};

            SLDataFormat_PCM dataFormat;
            dataFormat.formatType = SL_DATAFORMAT_PCM;
            dataFormat.numChannels = channels;
            dataFormat.samplesPerSec = sampleRate * 1000; //mHz
            dataFormat.bitsPerSample = sizeof(int16_t) * 8;
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

            SLDataSource dataSource = {&location, &dataFormat};

            SLDataLocator_OutputMix dataLocatorOut;
            dataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX;
            dataLocatorOut.outputMix = outputMixObject;

            SLDataSink dataSink;
            dataSink.pLocator = &dataLocatorOut;
            dataSink.pFormat = nullptr;

            constexpr SLuint32 playerIIDCount = 3;
            const SLInterfaceID playerIIDs[] = {SL_IID_BUFFERQUEUE, SL_IID_PLAY, SL_IID_VOLUME};
            constexpr SLboolean playerReqs[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

            if ((result = (*engine)->CreateAudioPlayer(engine, &playerObject, &dataSource, &dataSink, playerIIDCount, playerIIDs, playerReqs)) != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to create OpenSL player object");

            sampleFormat = SampleFormat::SINT16;

            if ((result = (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE)) != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to create OpenSL player object");

            if ((result = (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &player)) != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to get OpenSL player interface");

            if ((result = (*playerObject)->GetInterface(playerObject, SL_IID_BUFFERQUEUE, &bufferQueue)) != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to get OpenSL buffer queue interface");

            if ((result = (*playerObject)->GetInterface(playerObject, SL_IID_VOLUME, &playerVolume)) != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to get OpenSL volume interface");

            if ((result = (*bufferQueue)->Clear(bufferQueue)) != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to clear OpenSL buffer");

            if ((result = (*bufferQueue)->RegisterCallback(bufferQueue, playerCallback, this)) != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to register OpenSL buffer queue callback");

            getData(bufferSize / (channels * sizeof(int16_t)), data);

            if ((result = (*bufferQueue)->Enqueue(bufferQueue, data.data(), data.size())) != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to enqueue OpenSL data");

            if ((result = (*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING)) != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to play sound");
        }

        OSLAudioDevice::~OSLAudioDevice()
        {
            if (playerObject)
                (*playerObject)->Destroy(playerObject);

            if (outputMixObject)
                (*outputMixObject)->Destroy(outputMixObject);

            if (engineObject)
                (*engineObject)->Destroy(engineObject);
        }

        void OSLAudioDevice::enqueue(SLAndroidSimpleBufferQueueItf bufferQueue)
        {
            getData(bufferSize / (channels * sizeof(int16_t)), data);

            SLresult result;
            if ((result = (*bufferQueue)->Enqueue(bufferQueue, data.data(), data.size())) != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to enqueue OpenSL data");
        }
    } // namespace audio
} // namespace ouzel

#endif
