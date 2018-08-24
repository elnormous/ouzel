// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <queue>
#include <vector>
#include "audio/Audio.hpp"
#include "thread/Mutex.hpp"

namespace ouzel
{
    namespace audio
    {
        class ListenerResource;
        class MixerResource;

        class AudioDevice
        {
            friend Audio;
        public:
            virtual ~AudioDevice();

            AudioDevice(const AudioDevice&) = delete;
            AudioDevice& operator=(const AudioDevice&) = delete;

            AudioDevice(AudioDevice&&) = delete;
            AudioDevice& operator=(AudioDevice&&) = delete;

            virtual void process();

            inline uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            inline uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            struct RenderCommand
            {
                std::function<void(Vector3& listenerPosition,
                                   Quaternion& listenerRotation,
                                   float& pitch,
                                   float& gain,
                                   float& rolloffFactor)> attributeCallback;

                std::function<void(uint32_t frames,
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
            explicit AudioDevice(Audio::Driver initDriver);

            void getData(uint32_t frames, std::vector<uint8_t>& result);
            void processRenderCommands(uint32_t frames,
                                       std::vector<float>& result);
            void processRenderCommand(const RenderCommand& renderCommand,
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

            Audio::SampleFormat sampleFormat = Audio::SampleFormat::SINT16;
            const uint32_t bufferSize = 2 * 4096;
            const uint32_t sampleRate = 44100;
            const uint16_t channels = 2;

            std::vector<std::vector<float>> buffers;
            uint32_t currentBuffer = 0;

            std::vector<RenderCommand> renderQueue;
            Mutex renderQueueMutex;
        };
    } // namespace audio
} // namespace ouzel
