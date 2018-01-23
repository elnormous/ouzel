// Copyright (C) 2018 Elviss Strazdins
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
        class ListenerResource;
        class MixerResource;

        class AudioDevice: public Noncopyable
        {
            friend Audio;
        public:
            virtual ~AudioDevice();

            virtual bool process();

            inline uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            inline uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            void executeOnAudioThread(const std::function<void(void)>& func);

            struct RenderCommand
            {
                std::function<void(Vector3& listenerPosition,
                                   Quaternion& listenerRotation,
                                   float& pitch,
                                   float& gain,
                                   float& rolloffFactor)> attributeCallback;

                std::function<bool(uint32_t frames,
                                   uint16_t channels,
                                   uint32_t sampleRate,
                                   const Vector3& listenerPosition,
                                   const Quaternion& listenerRotation,
                                   float pitch,
                                   float gain,
                                   float rolloffFactor,
                                   std::vector<float>& result)> renderCallback;
                std::vector<RenderCommand> renderCommands;
            };

            void setRenderCommands(const std::vector<RenderCommand>& newRenderCommands);

        protected:
            AudioDevice(Audio::Driver aDriver);
            virtual bool init(bool debugAudio);

            void executeAll();
            bool getData(uint32_t frames, std::vector<uint8_t>& result);
            bool processRenderCommands(uint32_t frames,
                                       std::vector<float>& result);
            bool processRenderCommand(const RenderCommand& renderCommand,
                                      uint32_t frames,
                                      Vector3 listenerPosition,
                                      Quaternion listenerRotation,
                                      float pitch,
                                      float gain,
                                      float rolloffFactor,
                                      std::vector<float>& result);

            Audio::Driver driver;

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            Audio::Format format = Audio::Format::SINT16;
            const uint32_t bufferSize = 2 * 4096;
            const uint32_t sampleRate = 44100;
            const uint16_t channels = 2;

            std::vector<std::vector<float>> buffers;
            uint32_t currentBuffer = 0;

            std::queue<std::function<void(void)>> executeQueue;
            std::mutex executeMutex;

            std::vector<RenderCommand> renderQueue;
            std::mutex renderQueueMutex;
        };
    } // namespace audio
} // namespace ouzel
