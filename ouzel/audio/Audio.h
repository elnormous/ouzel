// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
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

            virtual ~Audio();

            virtual bool update();

            virtual SoundResource* createSound();
            virtual void deleteResource(SoundResource* resource);

            void setListenerPosition(const Vector3& newPosition);
            void setListenerRotation(const Quaternion& newRotation);

            uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

        protected:
            Audio(Driver aDriver);
            virtual bool init();

            std::vector<uint8_t> getData(uint32_t size);

            Driver driver;

            enum Dirty
            {
                DIRTY_LISTENER_POSITION = 0x01,
                DIRTY_LISTENER_ROTATION = 0x02
            };

            uint32_t dirty = 0;

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            Vector3 listenerPosition;
            Quaternion listenerRotation;

            std::mutex uploadMutex;

            std::mutex resourceMutex;
            std::vector<std::unique_ptr<SoundResource>> resources;
            std::vector<std::unique_ptr<SoundResource>> resourceDeleteSet;

            const uint32_t bufferSize = 2 * 4096;
            const uint32_t samplesPerSecond = 44100;
            const uint16_t channels = 2;
        };
    } // namespace audio
} // namespace ouzel
