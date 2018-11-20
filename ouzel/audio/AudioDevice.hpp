// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICE_HPP
#define OUZEL_AUDIO_AUDIODEVICE_HPP

#include <condition_variable>
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
                    DELETE_NODE,
                    UPDATE_NODE,
                    ADD_OUTPUT_NODE,
                    SET_DESTINATION_NODE
                };

                Command() {}
                explicit Command(Type initType): type(initType) {}

                Type type;
                uintptr_t nodeId;
                uintptr_t destinationNodeId;
                std::function<std::unique_ptr<Node>(void)> createFunction;
                std::function<void(Node*)> updateFunction;
            };

            explicit AudioDevice(Driver initDriver);
            virtual ~AudioDevice();

            AudioDevice(const AudioDevice&) = delete;
            AudioDevice& operator=(const AudioDevice&) = delete;

            AudioDevice(AudioDevice&&) = delete;
            AudioDevice& operator=(AudioDevice&&) = delete;

            inline uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            inline uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            virtual void process();

            void addCommand(const Command& command);

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

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;
            
            SampleFormat sampleFormat = SampleFormat::SINT16;
            const uint32_t bufferSize = 2 * 4096;
            const uint32_t sampleRate = 44100;
            const uint16_t channels = 2;

        private:
            std::vector<std::vector<float>> buffers;
            uint32_t currentBuffer = 0;

            uintptr_t lastNodeId = 0;
            std::queue<uintptr_t> deletedNodeIds;

            std::vector<std::unique_ptr<Node>> nodes;

            Driver driver;

            Node* destinationNode = nullptr;
            std::mutex commandMutex;
            std::condition_variable commandConditionVariable;
            std::queue<Command> commandQueue;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_AUDIODEVICE_HPP
