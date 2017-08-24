// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioDevice
        {
            friend Audio;
        public:
            virtual ~AudioDevice();

            virtual bool update();

            virtual SoundResource* createSound();
            virtual void deleteResource(SoundResource* resource);

            void setListenerPosition(const Vector3& newPosition);
            void setListenerRotation(const Quaternion& newRotation);

            uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            void executeOnAudioThread(const std::function<void(void)>& func);

        protected:
            AudioDevice(Audio::Driver aDriver);
            virtual bool init(bool debugAudio);

            void executeAll();
            bool getData(uint32_t samples, Audio::Format format, std::vector<uint8_t>& result);

            Audio::Driver driver;

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
