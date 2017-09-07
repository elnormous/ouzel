// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <mutex>
#include <queue>
#include <vector>
#include "utils/Noncopyable.hpp"
#include "audio/Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        class Resource;
        class ListenerResource;
        class MixerResource;
        class SoundResource;

        class AudioDevice: public Noncopyable
        {
            friend Audio;
        public:
            virtual ~AudioDevice();

            virtual bool process();

            SoundResource* createSound();
            void deleteResource(Resource* resource);

            uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            void executeOnAudioThread(const std::function<void(void)>& func);

            struct RenderCommand
            {
                std::function<void(const std::vector<uint8_t>&, const std::vector<uint8_t>&)> renderCallback;
                std::vector<RenderCommand> renderCommands;
            };

        protected:
            AudioDevice(Audio::Driver aDriver);
            virtual bool init(bool debugAudio);

            void executeAll();
            bool getData(uint32_t frames, Audio::Format format, std::vector<uint8_t>& result);

            Audio::Driver driver;

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            std::mutex resourceMutex;
            std::vector<std::unique_ptr<Resource>> resources;
            std::vector<std::unique_ptr<Resource>> resourceDeleteSet;

            const uint32_t bufferSize = 2 * 4096;
            const uint32_t sampleRate = 44100;
            const uint16_t channels = 2;

            std::vector<float> buffer;

            std::queue<std::function<void(void)>> executeQueue;
            std::mutex executeMutex;
        };
    } // namespace audio
} // namespace ouzel
