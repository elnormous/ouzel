// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENSL

#include <array>
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
                const auto audioDevice = static_cast<AudioDevice*>(context);
                audioDevice->enqueue(bufferQueue);
            }
            catch (const std::exception& e)
            {
                logger.log(Log::Level::error) << e.what();
            }
        }

        constexpr SLuint32 getChannelMask(std::uint32_t channels)
        {
            switch (channels)
            {
                case 1: return SL_SPEAKER_FRONT_CENTER;
                case 2: return SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
                case 4: return SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT | SL_SPEAKER_BACK_LEFT | SL_SPEAKER_BACK_RIGHT;
                case 6: return SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT | SL_SPEAKER_FRONT_CENTER | SL_SPEAKER_LOW_FREQUENCY | SL_SPEAKER_SIDE_LEFT|SL_SPEAKER_SIDE_RIGHT;
                case 7: return SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT | SL_SPEAKER_FRONT_CENTER | SL_SPEAKER_LOW_FREQUENCY | SL_SPEAKER_BACK_CENTER | SL_SPEAKER_SIDE_LEFT | SL_SPEAKER_SIDE_RIGHT;
                case 8: return SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT | SL_SPEAKER_FRONT_CENTER | SL_SPEAKER_LOW_FREQUENCY | SL_SPEAKER_BACK_LEFT|SL_SPEAKER_BACK_RIGHT | SL_SPEAKER_SIDE_LEFT | SL_SPEAKER_SIDE_RIGHT;
                default:
                    throw std::runtime_error("Invalid channel count");
            }
        }
    }

    AudioDevice::AudioDevice(const Settings& settings,
                             const std::function<void(std::uint32_t frames,
                                                      std::uint32_t channels,
                                                      std::uint32_t sampleRate,
                                                      std::vector<float>& samples)>& initDataGetter):
        audio::AudioDevice(Driver::openSl, settings, initDataGetter)
    {
        const std::array engineInterfaces = { SL_IID_ENGINE, SL_IID_ENGINECAPABILITIES };
        constexpr std::array engineRequirements = { SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE };

        SLObjectItf engineObjectPointer;
        if (const auto result = slCreateEngine(&engineObjectPointer, 0, nullptr,
                                               static_cast<SLuint32>(engineInterfaces.size()),
                                               engineInterfaces.data(),
                                               engineRequirements.data()); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to create OpenSL engine object");
        
        engineObject = engineObjectPointer;

        if (const auto result = engineObject->Realize(engineObject.get(), SL_BOOLEAN_FALSE); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to create OpenSL engine object");

        SLEngineCapabilitiesItf engineCapabilities;
        if (const auto result = engineObject->GetInterface(engineObject.get(), SL_IID_ENGINECAPABILITIES, &engineCapabilities); result == SL_RESULT_SUCCESS)
        {
            SLint16 major;
            SLint16 minor;
            SLint16 step;
            if (const auto result = (*engineCapabilities)->QueryAPIVersion(engineCapabilities, &major, &minor, &step); result != SL_RESULT_SUCCESS)
                throw std::system_error(makeErrorCode(result), "Failed to get OpenSL version");
            
            apiMajorVersion = static_cast<std::uint16_t>(major);
            apiMinorVersion = static_cast<std::uint16_t>(minor);
        }
        else if (result == SL_RESULT_FEATURE_UNSUPPORTED)
        {
            // engine capabilities interface is not supported in OpenSL ES 1.0
            apiMajorVersion = 1;
            apiMinorVersion = 0;
        }
        else
            throw std::system_error(makeErrorCode(result), "Failed to get OpenSL engine capabilities");

        if (const auto result = engineObject->GetInterface(engineObject.get(), SL_IID_ENGINE, &engine); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to get OpenSL engine");

        SLObjectItf outputMixObjectPointer;
        if (const auto result = (*engine)->CreateOutputMix(engine, &outputMixObjectPointer, 0, nullptr, nullptr); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to create OpenSL output mix");
        
        outputMixObject = outputMixObjectPointer;

        if (const auto result = outputMixObject->Realize(outputMixObject.get(), SL_BOOLEAN_FALSE); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to create OpenSL output mix");

        SLDataLocator_AndroidSimpleBufferQueue location = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};

        SLDataFormat_PCM dataFormat;
        dataFormat.formatType = SL_DATAFORMAT_PCM;
        // TODO: get speaker count
        dataFormat.numChannels = channels;
        dataFormat.samplesPerSec = sampleRate * 1000; // mHz
        dataFormat.bitsPerSample = sizeof(std::int16_t) * 8;
        dataFormat.containerSize = dataFormat.bitsPerSample;
        dataFormat.channelMask = getChannelMask(channels);
        dataFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;

        SLDataSource dataSource{&location, &dataFormat};
        SLDataLocator_OutputMix dataLocatorOut{SL_DATALOCATOR_OUTPUTMIX, outputMixObject.get()};
        SLDataSink dataSink{&dataLocatorOut, nullptr};
        constexpr SLuint32 playerIIDCount = 3;
        const SLInterfaceID playerIIDs[] = {SL_IID_BUFFERQUEUE, SL_IID_PLAY, SL_IID_VOLUME};
        constexpr SLboolean playerReqs[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

        SLObjectItf playerObjectPointer;
        if (const auto result = (*engine)->CreateAudioPlayer(engine, &playerObjectPointer, &dataSource, &dataSink, playerIIDCount, playerIIDs, playerReqs); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to create OpenSL player object");

        playerObject = playerObjectPointer;

        sampleFormat = SampleFormat::signedInt16;

        if (const auto result = playerObject->Realize(playerObject.get(), SL_BOOLEAN_FALSE); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to create OpenSL player object");

        if (const auto result = playerObject->GetInterface(playerObject.get(), SL_IID_PLAY, &player); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to get OpenSL player interface");

        if (const auto result = playerObject->GetInterface(playerObject.get(), SL_IID_BUFFERQUEUE, &bufferQueue); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to get OpenSL buffer queue interface");

        if (const auto result = playerObject->GetInterface(playerObject.get(), SL_IID_VOLUME, &playerVolume); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to get OpenSL volume interface");

        if (const auto result = (*bufferQueue)->Clear(bufferQueue); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to clear OpenSL buffer");

        if (const auto result = (*bufferQueue)->RegisterCallback(bufferQueue, playerCallback, this); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to register OpenSL buffer queue callback");
    }

    void AudioDevice::start()
    {
        getData(bufferSize / (channels * sizeof(std::int16_t)), data);

        if (const auto result = (*bufferQueue)->Enqueue(bufferQueue, data.data(), data.size()); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to enqueue OpenSL data");

        if (const auto result = (*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to play sound");
    }

    void AudioDevice::stop()
    {
        if (const auto result = (*player)->SetPlayState(player, SL_PLAYSTATE_STOPPED); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to stop sound");
    }

    void AudioDevice::enqueue(SLAndroidSimpleBufferQueueItf bufferQueue)
    {
        getData(bufferSize / (channels * sizeof(std::int16_t)), data);

        if (const auto result = (*bufferQueue)->Enqueue(bufferQueue, data.data(), data.size()); result != SL_RESULT_SUCCESS)
            throw std::system_error(makeErrorCode(result), "Failed to enqueue OpenSL data");
    }
}
#endif
