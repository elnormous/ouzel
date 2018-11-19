// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICE_HPP
#define OUZEL_AUDIO_AUDIODEVICE_HPP

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>
#include "audio/Driver.hpp"
#include "audio/Node.hpp"
#include "audio/SampleFormat.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class ListenerNode;
        class MixerNode;

        class AudioDevice
        {
            friend Audio;
        public:
            struct Command
            {
                enum class Type
                {
                    INIT_NODE,
                    DESTROY_NODE,
                    UPDATE_NODE,
                    SET_INPUT,
                    SET_OUTPUT,
                };

                Type type;
                uintptr_t resourceId;
                std::function<std::unique_ptr<Node>(void)> createFunction;
                std::function<void(Node*)> updateFunction;
            };

            explicit AudioDevice(Driver initDriver);
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

            uintptr_t getNodeId()
            {
                if (deletedNodeIds.empty())
                    return ++lastNodeId; // zero is reserved for null node
                else
                {
                    uintptr_t nodeId = deletedNodeIds.front();
                    deletedNodeIds.pop();
                    return nodeId;
                }
            }

            void deleteNodeId(uintptr_t nodeId)
            {
                deletedNodeIds.push(nodeId);
            }

        protected:
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

            Driver driver;

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            SampleFormat sampleFormat = SampleFormat::SINT16;
            const uint32_t bufferSize = 2 * 4096;
            const uint32_t sampleRate = 44100;
            const uint16_t channels = 2;

            std::vector<std::vector<float>> buffers;
            uint32_t currentBuffer = 0;

            std::vector<RenderCommand> renderQueue;
            std::mutex renderQueueMutex;

            uintptr_t lastNodeId = 0;
            std::queue<uintptr_t> deletedNodeIds;

            std::vector<std::unique_ptr<Node>> nodes;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_AUDIODEVICE_HPP
