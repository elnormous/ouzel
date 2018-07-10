// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENAL

#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include <objc/message.h>
extern "C" id const AVAudioSessionCategoryAmbient;
#endif

#include "AudioDeviceAL.hpp"
#include "core/Engine.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace audio
    {
        AudioDeviceAL::AudioDeviceAL():
            AudioDevice(Audio::Driver::OPENAL)
        {
#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
            id audioSession = reinterpret_cast<id (*)(Class, SEL)>(&objc_msgSend)(objc_getClass("AVAudioSession"), sel_getUid("sharedInstance"));
            reinterpret_cast<BOOL (*)(id, SEL, id, id)>(&objc_msgSend)(audioSession, sel_getUid("setCategory:error:"), AVAudioSessionCategoryAmbient, nil);
#endif

            std::fill(std::begin(bufferIds), std::end(bufferIds), 0);

            const ALCchar* deviceName = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);

            if (deviceName)
                Log(Log::Level::INFO) << "Using " << deviceName << " for audio";

            device = alcOpenDevice(deviceName);

            if (!device)
                throw SystemError("Failed to create OpenAL device");

            ALCenum alcError;

            if ((alcError = alcGetError(device)) != ALC_NO_ERROR)
                throw SystemError("Failed to create OpenAL device, error: " + std::to_string(alcError));

            context = alcCreateContext(device, nullptr);

            if ((alcError = alcGetError(device)) != ALC_NO_ERROR)
                throw SystemError("Failed to create OpenAL context, error: " + std::to_string(alcError));

            if (!context)
                throw SystemError("Failed to create OpenAL context");

            alcMakeContextCurrent(context);

            if ((alcError = alcGetError(device)) != ALC_NO_ERROR)
                throw SystemError("Failed to make OpenAL context current, error: " + std::to_string(alcError));

            ALenum error;

#if !OUZEL_PLATFORM_EMSCRIPTEN
            format40 = alGetEnumValue("AL_FORMAT_QUAD16");
            format51 = alGetEnumValue("AL_FORMAT_51CHN16");
            format61 = alGetEnumValue("AL_FORMAT_61CHN16");
            format71 = alGetEnumValue("AL_FORMAT_71CHN16");

            if ((error = alGetError()) != AL_NO_ERROR)
                Log(Log::Level::WARN) << "Failed to get OpenAL enum values";
#endif

            alGenSources(1, &sourceId);

            if ((error = alGetError()) != AL_NO_ERROR)
                throw SystemError("Failed to create OpenAL source, error: " + std::to_string(error));

            alGenBuffers(2, bufferIds);

            if ((error = alGetError()) != AL_NO_ERROR)
                throw SystemError("Failed to create OpenAL buffers, error: " + std::to_string(error));

            switch (channels)
            {
                case 1: format = AL_FORMAT_MONO16; break;
                case 2: format = AL_FORMAT_STEREO16; break;
                case 4: format = format40; break;
                case 6: format = format51; break;
                case 7: format = format61; break;
                case 8: format = format71; break;
                default:
                    throw SystemError("Invalid channel count");
            }

            sampleFormat = Audio::SampleFormat::SINT16;

            getData(bufferSize / (channels * sizeof(int16_t)), data);

            alBufferData(bufferIds[0], format,
                         data.data(),
                         static_cast<ALsizei>(data.size()),
                         static_cast<ALsizei>(sampleRate));

            getData(bufferSize / (channels * sizeof(int16_t)), data);

            alBufferData(bufferIds[1], format,
                         data.data(),
                         static_cast<ALsizei>(data.size()),
                         static_cast<ALsizei>(sampleRate));

            nextBuffer = 0;

            alSourceQueueBuffers(sourceId, 2, bufferIds);

            if ((error = alGetError()) != AL_NO_ERROR)
                throw SystemError("Failed to queue OpenAL buffers, error: " + std::to_string(error));

            alSourcePlay(sourceId);

            if ((error = alGetError()) != AL_NO_ERROR)
                throw SystemError("Failed to play OpenAL source, error: " + std::to_string(error));

#if OUZEL_MULTITHREADED
            running = true;
            audioThread = Thread(std::bind(&AudioDeviceAL::run, this), "Audio");
#endif
        }

        AudioDeviceAL::~AudioDeviceAL()
        {
#if OUZEL_MULTITHREADED
            running = false;
            if (audioThread.isJoinable()) audioThread.join();
#endif

            if (sourceId)
            {
                alSourceStop(sourceId);
                alSourcei(sourceId, AL_BUFFER, 0);
                alDeleteSources(1, &sourceId);
                alGetError();
            }

            for (ALuint bufferId : bufferIds)
            {
                if (bufferId)
                {
                    alDeleteBuffers(1, &bufferId);
                    alGetError();
                }
            }

            if (context)
            {
                alcMakeContextCurrent(nullptr);
                alcDestroyContext(context);
            }

            if (device)
                alcCloseDevice(device);
        }

        void AudioDeviceAL::process()
        {
            AudioDevice::process();

            alcMakeContextCurrent(context);

            ALCenum alcError;

            if ((alcError = alcGetError(device)) != ALC_NO_ERROR)
                throw SystemError("Failed to make OpenAL context current, error: " + std::to_string(alcError));

            ALint buffersProcessed;
            alGetSourcei(sourceId, AL_BUFFERS_PROCESSED, &buffersProcessed);

            ALenum error;

            if ((error = alGetError()) != AL_NO_ERROR)
                throw SystemError("Failed to get processed buffer count, error: " + std::to_string(error));

            // requeue all processed buffers
            for (; buffersProcessed > 0; --buffersProcessed)
            {
                alSourceUnqueueBuffers(sourceId, 1, &bufferIds[nextBuffer]);

                if ((error = alGetError()) != AL_NO_ERROR)
                    throw SystemError("Failed to unqueue OpenAL buffer, error: " + std::to_string(error));

                getData(bufferSize / (channels * sizeof(int16_t)), data);

                alBufferData(bufferIds[nextBuffer], format,
                             data.data(),
                             static_cast<ALsizei>(data.size()),
                             static_cast<ALsizei>(sampleRate));

                alSourceQueueBuffers(sourceId, 1, &bufferIds[nextBuffer]);

                if ((error = alGetError()) != AL_NO_ERROR)
                    throw SystemError("Failed to queue OpenAL buffer, error: " + std::to_string(error));

                ALint state;
                alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
                if (state != AL_PLAYING)
                {
                    alSourcePlay(sourceId);

                    if ((error = alGetError()) != AL_NO_ERROR)
                        throw SystemError("Failed to play OpenAL source, error: " + std::to_string(error));
                }

                // swap the buffer
                nextBuffer = (nextBuffer + 1) % 2;
            }
        }

        void AudioDeviceAL::run()
        {
#if OUZEL_MULTITHREADED
            while (running) process();
#endif
        }
    } // namespace audio
} // namespace ouzel

#endif
