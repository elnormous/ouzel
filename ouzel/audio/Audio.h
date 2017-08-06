// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <vector>
#include "math/Quaternion.h"
#include "math/Vector3.h"

namespace ouzel
{
    class Engine;

    namespace audio
    {
        class Resource;
        class SoundResource;

        class Audio
        {
            friend Engine;
        public:
            const uint32_t SPEAKER_COUNT = 6;

            enum class Driver
            {
                DEFAULT,
                EMPTY,
                OPENAL,
                DIRECTSOUND,
                XAUDIO2,
                OPENSL,
                COREAUDIO,
                ALSA
            };

            enum Channel
            {
                CHANNEL_FRONT_LEFT = 0,
                CHANNEL_FRONT_RIGHT = 1,
                CHANNEL_CENTER = 2,
                CHANNEL_LFE = 3,
                CHANNEL_BACK_LEFT = 4,
                CHANNEL_BACK_RIGHT = 5,
                CHANNEL_SIDE_LEFT = 6,
                CHANNEL_SIDE_RIGHT = 7
            };

            enum class Format
            {
                SINT16,
                FLOAT32
            };

            virtual ~Audio();

            virtual bool update();

            virtual SoundResource* createSound();
            virtual void deleteResource(SoundResource* resource);

            void setListenerPosition(const Vector3& newPosition);
            void setListenerRotation(const Quaternion& newRotation);

            uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            void executeOnAudioThread(const std::function<void(void)>& func);

        protected:
            Audio(Driver aDriver);
            virtual bool init(bool debugAudio);

            void executeAll();

            bool getData(uint32_t samples, Format format, std::vector<uint8_t>& result);

            Driver driver;

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            Vector3 listenerPosition;
            Quaternion listenerRotation;

            std::mutex uploadMutex;

            std::mutex resourceMutex;
            std::vector<std::unique_ptr<SoundResource>> resources;
            std::vector<std::unique_ptr<SoundResource>> resourceDeleteSet;

            const uint32_t bufferSize = 2 * 4096;
            const uint32_t sampleRate = 44100;
            const uint16_t channels = 2;

            std::vector<float> buffer;

            std::queue<std::function<void(void)>> executeQueue;
            std::mutex executeMutex;
        };
    } // namespace audio
} // namespace ouzel
