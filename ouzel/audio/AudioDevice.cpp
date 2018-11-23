// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "AudioDevice.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        AudioDevice::AudioDevice(Driver initDriver):
            driver(initDriver)
        {
            buffers.resize(1000);
        }

        AudioDevice::~AudioDevice()
        {
        }

        void AudioDevice::process()
        {
            Command command;
            for (;;)
            {
                std::unique_lock<std::mutex> lock(commandMutex);
                if (commandQueue.empty()) break;
                command = std::move(commandQueue.front());
                commandQueue.pop();
                lock.unlock();

                switch (command.type)
                {
                    case Command::Type::INIT_NODE:
                    {
                        if (command.nodeId > nodes.size())
                            nodes.resize(command.nodeId);

                        nodes[command.nodeId - 1] = command.createFunction();
                        break;
                    }
                    case Command::Type::DELETE_NODE:
                    {
                        nodes[command.nodeId - 1].reset();
                        break;
                    }
                    case Command::Type::UPDATE_NODE:
                    {
                        command.updateFunction(nodes[command.nodeId - 1].get());
                        break;
                    }
                    case Command::Type::ADD_OUTPUT_NODE:
                    {
                        nodes[command.nodeId - 1]->addOutputNode(nodes[command.destinationNodeId - 1].get());
                        break;
                    }
                    case Command::Type::SET_DESTINATION_NODE:
                    {
                        destinationNode = command.nodeId ? nodes[command.nodeId - 1].get() : nullptr;
                        break;
                    }
                    default:
                        throw std::runtime_error("Invalid command");
                }
            }
        }

        void AudioDevice::getData(uint32_t frames, std::vector<uint8_t>& result)
        {
            currentBuffer = 0;
            uint32_t buffer = currentBuffer;

            if (++currentBuffer > buffers.size()) return; // out of buffers

            buffers[buffer].resize(frames * channels);
            std::fill(buffers[buffer].begin(), buffers[buffer].end(), 0.0F);

            if (destinationNode)
            {
                uint16_t inputChannels = channels;
                uint32_t inputSampleRate = sampleRate;
                Vector3 inputPosition;

                destinationNode->process(buffers[buffer], inputChannels, inputSampleRate, inputPosition);
            }

            for (float& f : buffers[buffer])
                f = clamp(f, -1.0F, 1.0F);

            switch (sampleFormat)
            {
                case SampleFormat::SINT16:
                {
                    result.resize(frames * channels * sizeof(int16_t));
                    int16_t* resultPtr = reinterpret_cast<int16_t*>(result.data());

                    for (uint32_t i = 0; i < buffers[buffer].size(); ++i)
                    {
                        *resultPtr = static_cast<int16_t>(buffers[buffer][i] * 32767.0F);
                        ++resultPtr;
                    }
                    break;
                }
                case SampleFormat::FLOAT32:
                {
                    result.reserve(frames * channels * sizeof(float));
                    result.assign(reinterpret_cast<uint8_t*>(buffers[buffer].data()),
                                  reinterpret_cast<uint8_t*>(buffers[buffer].data()) + buffers[buffer].size() * sizeof(float));
                    break;
                }
                default:
                    throw std::runtime_error("Invalid sample format");
            }
        }

        void AudioDevice::addCommand(const Command& command)
        {
            std::unique_lock<std::mutex> lock(commandMutex);
            commandQueue.push(command);
            lock.unlock();
            commandConditionVariable.notify_all();
        }
    } // namespace audio
} // namespace ouzel
