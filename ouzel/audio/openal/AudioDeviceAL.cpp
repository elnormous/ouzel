// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENAL

#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include <objc/message.h>
extern "C" id const AVAudioSessionCategoryAmbient;
#endif

#include "AudioDeviceAL.hpp"
#include "core/Engine.hpp"

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

#if OUZEL_MULTITHREADED
            running = false;
#endif

            std::fill(std::begin(bufferIds), std::end(bufferIds), 0);
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

                if (checkOpenALError())
                    Log(Log::Level::ERR) << "Failed to delete OpenAL source";
            }

            for (ALuint bufferId : bufferIds)
            {
                if (bufferId)
                {
                    alDeleteBuffers(1, &bufferId);

                    if (checkOpenALError())
                        Log(Log::Level::ERR) << "Failed to delete OpenAL buffer";
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

        bool AudioDeviceAL::init(bool debugAudio)
        {
            if (!AudioDevice::init(debugAudio))
                return false;

            const ALCchar* deviceName = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);

            if (deviceName)
                Log(Log::Level::INFO) << "Using " << deviceName << " for audio";

            device = alcOpenDevice(deviceName);

            if (!device || checkALCError())
            {
                Log(Log::Level::ERR) << "Failed to create OpenAL device";
                return false;
            }

            int capabilities[] =
            {
                ALC_FREQUENCY, 44100,
                ALC_STEREO_SOURCES, 4,
                0, 0
            };

            context = alcCreateContext(device, capabilities);

            if (checkALCError())
            {
                Log(Log::Level::ERR) << "Failed to create OpenAL context";
                return false;
            }

            alcMakeContextCurrent(context);

            if (checkALCError())
            {
                Log(Log::Level::ERR) << "Failed to make OpenAL context current";
                return false;
            }

#if !OUZEL_PLATFORM_EMSCRIPTEN
            format40 = alGetEnumValue("AL_FORMAT_QUAD16");
            format51 = alGetEnumValue("AL_FORMAT_51CHN16");
            format61 = alGetEnumValue("AL_FORMAT_61CHN16");
            format71 = alGetEnumValue("AL_FORMAT_71CHN16");
#endif

            if (checkOpenALError())
                Log(Log::Level::WARN) << "Failed to get OpenAL enum values";

            alGenSources(1, &sourceId);

            if (checkOpenALError())
            {
                Log(Log::Level::ERR) << "Failed to create OpenAL source";
                return false;
            }

            alGenBuffers(2, bufferIds);

            if (checkOpenALError())
            {
                Log(Log::Level::ERR) << "Failed to create OpenAL buffers";
                return false;
            }

            switch (channels)
            {
                case 1: format = AL_FORMAT_MONO16; break;
                case 2: format = AL_FORMAT_STEREO16; break;
                case 4: format = format40; break;
                case 6: format = format51; break;
                case 7: format = format61; break;
                case 8: format = format71; break;
                default:
                {
                    Log(Log::Level::ERR) << "Invalid channel count";
                    return false;
                }
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

            if (checkOpenALError())
            {
                Log(Log::Level::ERR) << "Failed to queue OpenAL buffers";
                return false;
            }

            alSourcePlay(sourceId);

            if (checkOpenALError())
            {
                Log(Log::Level::ERR) << "Failed to play OpenAL source";
                return false;
            }

#if OUZEL_MULTITHREADED
            running = true;
            audioThread = Thread(std::bind(&AudioDeviceAL::run, this), "Audio");
#endif

            return true;
        }

        bool AudioDeviceAL::process()
        {
            if (!AudioDevice::process())
                return false;

            alcMakeContextCurrent(context);

            if (checkALCError())
            {
                Log(Log::Level::ERR) << "Failed to make OpenAL context current";
                return false;
            }

            ALint buffersProcessed;
            alGetSourcei(sourceId, AL_BUFFERS_PROCESSED, &buffersProcessed);

            if (checkOpenALError())
            {
                Log(Log::Level::ERR) << "Failed to get processed buffer count";
                return false;
            }

            // requeue all processed buffers
            for (; buffersProcessed > 0; --buffersProcessed)
            {
                alSourceUnqueueBuffers(sourceId, 1, &bufferIds[nextBuffer]);

                if (checkOpenALError())
                {
                    Log(Log::Level::ERR) << "Failed to unqueue OpenAL buffer";
                    return false;
                }

                if (!getData(bufferSize / (channels * sizeof(int16_t)), data))
                    return false;

                alBufferData(bufferIds[nextBuffer], format,
                             data.data(),
                             static_cast<ALsizei>(data.size()),
                             static_cast<ALsizei>(sampleRate));

                alSourceQueueBuffers(sourceId, 1, &bufferIds[nextBuffer]);

                if (checkOpenALError())
                {
                    Log(Log::Level::ERR) << "Failed to queue OpenAL buffer";
                    return false;
                }

                ALint state;
                alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
                if (state != AL_PLAYING)
                {
                    alSourcePlay(sourceId);

                    if (checkOpenALError())
                    {
                        Log(Log::Level::ERR) << "Failed to play OpenAL source";
                        return false;
                    }
                }

                // swap the buffer
                nextBuffer = (nextBuffer + 1) % 2;
            }

            return true;
        }

        void AudioDeviceAL::run()
        {
#if OUZEL_MULTITHREADED
            while (running)
                if (!process()) break;
#endif
        }
    } // namespace audio
} // namespace ouzel

#endif
