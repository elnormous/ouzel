// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENAL

#ifdef __APPLE__
#  include <TargetConditionals.h>
#endif
#if TARGET_OS_IOS || TARGET_OS_TV
#  include <objc/message.h>
#  include <objc/NSObjCRuntime.h>
extern "C" id const AVAudioSessionCategoryAmbient;
#endif

#include "OALAudioDevice.hpp"
#include "OALErrorCategory.hpp"
#include "ALCErrorCategory.hpp"
#include "../../core/Engine.hpp"
#include "../../utils/Log.hpp"

#ifndef AL_FORMAT_MONO_FLOAT32
#  define AL_FORMAT_MONO_FLOAT32 0x10010
#endif

#ifndef AL_FORMAT_STEREO_FLOAT32
#  define AL_FORMAT_STEREO_FLOAT32 0x10011
#endif

namespace ouzel::audio::openal
{
    namespace
    {
        const ErrorCategory openAlErrorCategory{};
        const alc::ErrorCategory alcErrorCategory{};
    }

    AudioDevice::AudioDevice(const Settings& settings,
                             const std::function<void(std::uint32_t frames,
                                                      std::uint32_t channels,
                                                      std::uint32_t sampleRate,
                                                      std::vector<float>& samples)>& initDataGetter):
        audio::AudioDevice{Driver::openAl, settings, initDataGetter}
    {
#if TARGET_OS_IOS || TARGET_OS_TV
        id audioSession = reinterpret_cast<id (*)(Class, SEL)>(&objc_msgSend)(objc_getClass("AVAudioSession"), sel_getUid("sharedInstance")); // [AVAudioSession sharedInstance]
        if (!reinterpret_cast<BOOL (*)(id, SEL, id, id)>(&objc_msgSend)(audioSession, sel_getUid("setCategory:error:"), AVAudioSessionCategoryAmbient, nil)) // [audioSession setCategory:AVAudioSessionCategoryAmbient error:nil]
            throw std::runtime_error("Failed to set audio session category");

        id currentRoute = reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(audioSession, sel_getUid("currentRoute")); // [audioSession currentRoute]
        id outputs = reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(currentRoute, sel_getUid("outputs")); // [currentRoute outputs]
        const auto count = reinterpret_cast<NSUInteger (*)(id, SEL)>(&objc_msgSend)(outputs, sel_getUid("count")); // [outputs count]

        NSUInteger maxChannelCount = 0;
        for (NSUInteger outputIndex = 0; outputIndex < count; ++outputIndex)
        {
            id output = reinterpret_cast<id (*)(id, SEL, NSUInteger)>(&objc_msgSend)(outputs, sel_getUid("objectAtIndex:"), outputIndex); // [outputs objectAtIndex:outputIndex]
            id channels = reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(output, sel_getUid("channels")); // [output channels]
            const auto channelCount = reinterpret_cast<NSUInteger (*)(id, SEL)>(&objc_msgSend)(channels, sel_getUid("count")); // [channels count]
            if (channelCount > maxChannelCount)
                maxChannelCount = channelCount;
        }

        if (channels > maxChannelCount)
            channels = static_cast<std::uint32_t>(maxChannelCount);
#endif

        const auto deviceName = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);

        logger.log(Log::Level::info) << "Using " << deviceName << " for audio";

        device = alcOpenDevice(deviceName);

        if (!device)
            throw std::runtime_error("Failed to open ALC device");

        if (const auto error = alcGetError(device); error != ALC_NO_ERROR)
            throw std::system_error(error, alcErrorCategory, "Failed to create ALC device");

        ALCint majorVersion;
        alcGetIntegerv(device, ALC_MAJOR_VERSION, sizeof(majorVersion), &majorVersion);
        if (const auto error = alcGetError(device); error != ALC_NO_ERROR)
            throw std::system_error(error, alcErrorCategory, "Failed to get major version");

        apiMajorVersion = static_cast<std::uint16_t>(majorVersion);

        ALCint minorVersion;
        alcGetIntegerv(device, ALC_MINOR_VERSION, sizeof(minorVersion), &minorVersion);
        if (const auto error = alcGetError(device); error != ALC_NO_ERROR)
            throw std::system_error(error, alcErrorCategory, "Failed to get minor version");

        apiMinorVersion = static_cast<std::uint16_t>(minorVersion);

        logger.log(Log::Level::info) << "OpenAL version " << apiMajorVersion << '.' << apiMinorVersion;

        context = alcCreateContext(device, nullptr);

        if (const auto error = alcGetError(device); error != ALC_NO_ERROR)
            throw std::system_error(error, alcErrorCategory, "Failed to create ALC context");

        if (!context)
            throw std::runtime_error("Failed to create ALC context");

        if (!alcMakeContextCurrent(context))
            throw std::runtime_error("Failed to make ALC context current");

        if (const auto error = alcGetError(device); error != ALC_NO_ERROR)
            throw std::system_error(error, alcErrorCategory, "Failed to make ALC context current");

        std::string rendererName;
        const auto rendererNamePointer = alGetString(AL_RENDERER);

        if (const auto error = alGetError(); error != AL_NO_ERROR)
            logger.log(Log::Level::warning) << "Failed to get OpenAL renderer, error: " + std::to_string(error);
        else if (!rendererNamePointer)
            logger.log(Log::Level::warning) << "Failed to get OpenAL renderer";
        else
            rendererName = rendererNamePointer;

        std::string vendorName;
        const auto vendorNamePointer = alGetString(AL_VENDOR);

        if (const auto error = alGetError(); error != AL_NO_ERROR)
            logger.log(Log::Level::warning) << "Failed to get OpenAL renderer's vendor, error: " + std::to_string(error);
        else if (!vendorNamePointer)
            logger.log(Log::Level::warning) << "Failed to get OpenAL renderer's vendor";
        else
            vendorName = vendorNamePointer;

        logger.log(Log::Level::info) << "Using " << rendererName << " by " << vendorName << " audio renderer";

        std::vector<std::string> extensions;
        const auto extensionsPtr = alGetString(AL_EXTENSIONS);

        if (const auto error = alGetError(); error != AL_NO_ERROR)
            logger.log(Log::Level::warning) << "Failed to get OpenGL extensions: " + std::to_string(error);
        else if (!extensionsPtr)
            logger.log(Log::Level::warning) << "Failed to get OpenGL extensions";
        else
            extensions = explodeString(std::string(extensionsPtr), ' ');

        logger.log(Log::Level::all) << "Supported OpenAL extensions: " << extensions;

        auto float32Supported = false;
        for (const std::string& extension : extensions)
        {
            if (extension == "AL_EXT_float32")
                float32Supported = true;
        }

        alGenSources(1, &sourceId);

        if (const auto error = alGetError(); error != AL_NO_ERROR)
            throw std::system_error(error, openAlErrorCategory, "Failed to create OpenAL source");

        alGenBuffers(static_cast<ALsizei>(bufferIds.size()),
                     bufferIds.data());

        if (const auto error = alGetError(); error != AL_NO_ERROR)
            throw std::system_error(error, openAlErrorCategory, "Failed to create OpenAL buffers");

        switch (channels)
        {
            case 1:
            {
                if (float32Supported)
                {
                    format = AL_FORMAT_MONO_FLOAT32;
                    sampleFormat = SampleFormat::float32;
                    sampleSize = sizeof(float);
                }
                else
                {
                    format = AL_FORMAT_MONO16;
                    sampleFormat = SampleFormat::signedInt16;
                    sampleSize = sizeof(std::int16_t);
                }
                break;
            }
            case 2:
            {
                if (float32Supported)
                {
                    format = AL_FORMAT_STEREO_FLOAT32;
                    sampleFormat = SampleFormat::float32;
                    sampleSize = sizeof(float);
                }
                else
                {
                    format = AL_FORMAT_STEREO16;
                    sampleFormat = SampleFormat::signedInt16;
                    sampleSize = sizeof(std::int16_t);
                }
                break;
            }
            case 4:
            {
                format = alGetEnumValue("AL_FORMAT_QUAD16");
                if (const auto error = alGetError(); error != AL_NO_ERROR)
                    throw std::system_error(error, openAlErrorCategory, "Failed to create OpenAL enum value");
                sampleFormat = SampleFormat::signedInt16;
                sampleSize = sizeof(std::int16_t);
                break;
            }
            case 6:
            {
                format = alGetEnumValue("AL_FORMAT_51CHN16");
                if (const auto error = alGetError(); error != AL_NO_ERROR)
                    throw std::system_error(error, openAlErrorCategory, "Failed to create OpenAL enum value");
                sampleFormat = SampleFormat::signedInt16;
                sampleSize = sizeof(std::int16_t);
                break;
            }
            case 7:
            {
                format = alGetEnumValue("AL_FORMAT_61CHN16");
                if (const auto error = alGetError(); error != AL_NO_ERROR)
                    throw std::system_error(error, openAlErrorCategory, "Failed to create OpenAL enum value");
                sampleFormat = SampleFormat::signedInt16;
                sampleSize = sizeof(std::int16_t);
                break;
            }
            case 8:
            {
                format = alGetEnumValue("AL_FORMAT_71CHN16");
                if (const auto error = alGetError(); error != AL_NO_ERROR)
                    throw std::system_error(error, openAlErrorCategory, "Failed to create OpenAL enum value");
                sampleFormat = SampleFormat::signedInt16;
                sampleSize = sizeof(std::int16_t);
                break;
            }
            default:
                throw std::runtime_error("Invalid channel count");
        }
    }

    AudioDevice::~AudioDevice()
    {
#ifndef __EMSCRIPTEN__
        running = false;
        if (audioThread.isJoinable()) audioThread.join();
#endif

        if (context)
        {
            alcMakeContextCurrent(context);

            if (sourceId)
            {
                alSourceStop(sourceId);
                alSourcei(sourceId, AL_BUFFER, 0);
                alDeleteSources(1, &sourceId);
                alGetError();
            }

            for (const auto bufferId : bufferIds)
            {
                if (bufferId)
                {
                    alDeleteBuffers(1, &bufferId);
                    alGetError();
                }
            }

            alcMakeContextCurrent(nullptr);
            alcDestroyContext(context);
        }

        if (device)
            alcCloseDevice(device);
    }

    void AudioDevice::start()
    {
        getData(bufferSize / (channels * sampleSize), data);

        alBufferData(bufferIds[0], format,
                     data.data(),
                     static_cast<ALsizei>(data.size()),
                     static_cast<ALsizei>(sampleRate));

        getData(bufferSize / (channels * sampleSize), data);

        alBufferData(bufferIds[1], format,
                     data.data(),
                     static_cast<ALsizei>(data.size()),
                     static_cast<ALsizei>(sampleRate));

        alSourceQueueBuffers(sourceId,
                             static_cast<ALsizei>(bufferIds.size()),
                             bufferIds.data());

        if (const auto error = alGetError(); error != AL_NO_ERROR)
            throw std::system_error(error, openAlErrorCategory, "Failed to queue OpenAL buffers");

        alSourcePlay(sourceId);

        if (const auto error = alGetError(); error != AL_NO_ERROR)
            throw std::system_error(error, openAlErrorCategory, "Failed to play OpenAL source");

        if (!alcMakeContextCurrent(nullptr))
            throw std::runtime_error("Failed to unset current ALC context");

#ifndef __EMSCRIPTEN__
        running = true;
        audioThread = thread::Thread(&AudioDevice::run, this);
#endif
    }

    void AudioDevice::stop()
    {
#ifndef __EMSCRIPTEN__
        running = false;
        if (audioThread.isJoinable()) audioThread.join();
#endif

        if (!alcMakeContextCurrent(context))
            throw std::runtime_error("Failed to make ALC context current");

        if (const auto error = alcGetError(device); error != ALC_NO_ERROR)
            throw std::system_error(error, alcErrorCategory, "Failed to make ALC context current");

        alSourceStop(sourceId);

        if (const auto error = alGetError(); error != AL_NO_ERROR)
            throw std::system_error(error, openAlErrorCategory, "Failed to stop OpenAL source");
    }

    void AudioDevice::process()
    {
        ALint buffersProcessed;
        alGetSourcei(sourceId, AL_BUFFERS_PROCESSED, &buffersProcessed);

        if (const auto error = alGetError(); error != AL_NO_ERROR)
            throw std::system_error(error, openAlErrorCategory, "Failed to get processed buffer count");

        // requeue all processed buffers
        for (ALint i = 0; i < buffersProcessed; ++i)
        {
            ALuint buffer;
            alSourceUnqueueBuffers(sourceId, 1, &buffer);

            if (const auto error = alGetError(); error != AL_NO_ERROR)
                throw std::system_error(error, openAlErrorCategory, "Failed to unqueue OpenAL buffer");

            getData(bufferSize / (channels * sampleSize), data);

            alBufferData(buffer, format,
                         data.data(),
                         static_cast<ALsizei>(data.size()),
                         static_cast<ALsizei>(sampleRate));

            alSourceQueueBuffers(sourceId, 1, &buffer);

            if (const auto error = alGetError(); error != AL_NO_ERROR)
                throw std::system_error(error, openAlErrorCategory, "Failed to queue OpenAL buffer");
        }

        if (buffersProcessed == static_cast<ALint>(bufferIds.size()))
        {
            ALint state;
            alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING)
            {
                alSourcePlay(sourceId);

                if (const auto error = alGetError(); error != AL_NO_ERROR)
                    throw std::system_error(error, openAlErrorCategory, "Failed to play OpenAL source");
            }
        }
    }

    void AudioDevice::run()
    {
        thread::setCurrentThreadName("Audio");

        if (!alcMakeContextCurrent(context))
            throw std::runtime_error("Failed to make ALC context current");

        if (const auto error = alcGetError(device); error != ALC_NO_ERROR)
            throw std::system_error(error, alcErrorCategory, "Failed to make ALC context current");

#ifndef __EMSCRIPTEN__
        while (running)
        {
            try
            {
                process();
            }
            catch (const std::exception& e)
            {
                ouzel::logger.log(ouzel::Log::Level::error) << e.what();
            }
        }
#endif
    }
}
#endif
