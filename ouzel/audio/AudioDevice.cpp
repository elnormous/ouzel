// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "AudioDevice.hpp"
#include "math/MathUtils.hpp"
#include "thread/Lock.hpp"

namespace ouzel
{
    namespace audio
    {
        AudioDevice::AudioDevice(Audio::Driver initDriver):
            driver(initDriver)
        {
            buffers.resize(1000);
        }

        AudioDevice::~AudioDevice()
        {
        }

        bool AudioDevice::init(bool)
        {
            return true;
        }

        bool AudioDevice::process()
        {
            executeAll();

            return true;
        }

        void AudioDevice::setRenderCommands(const std::vector<RenderCommand>& newRenderCommands)
        {
            Lock renderQueueLock(renderQueueMutex);

            renderQueue = newRenderCommands;
        }

        bool AudioDevice::processRenderCommands(uint32_t frames, std::vector<float>& result)
        {
            std::vector<RenderCommand> renderCommands;

            {
                Lock renderQueueLock(renderQueueMutex);

                renderCommands = renderQueue;
            }

            uint32_t buffer = currentBuffer;
            if (++currentBuffer > buffers.size()) return true; // out of buffers

            buffers[buffer].resize(frames * channels);
            std::fill(buffers[buffer].begin(), buffers[buffer].end(), 0.0F);

            for (const RenderCommand& renderCommand : renderCommands)
            {
                if (!processRenderCommand(renderCommand,
                                          frames,
                                          Vector3(), // listener position
                                          Quaternion(), // listener rotation
                                          1.0F, // pitch
                                          1.0F, // gain
                                          1.0F, // rolloff factor
                                          buffers[buffer])) return false;

                if (buffers[buffer].size() > result.size()) result.resize(buffers[buffer].size(), 0.0F);

                for (uint32_t i = 0; i < buffers[buffer].size() && i < result.size(); ++i)
                {
                    // mix the sound into the buffer
                    result[i] += buffers[buffer][i];
                }
            }

            return true;
        }

        bool AudioDevice::processRenderCommand(const RenderCommand& renderCommand,
                                               uint32_t frames,
                                               Vector3 listenerPosition,
                                               Quaternion listenerRotation,
                                               float pitch,
                                               float gain,
                                               float rolloffFactor,
                                               std::vector<float>& result)
        {
            uint32_t buffer = currentBuffer;

            if (++currentBuffer > buffers.size()) return true; // out of buffers

            buffers[buffer].resize(frames * channels);
            std::fill(buffers[buffer].begin(), buffers[buffer].end(), 0.0F);

            if (renderCommand.attributeCallback)
            {
                renderCommand.attributeCallback(listenerPosition,
                                                listenerRotation,
                                                pitch,
                                                gain,
                                                rolloffFactor);
            }

            for (const RenderCommand& command : renderCommand.renderCommands)
            {
                processRenderCommand(command,
                                     frames,
                                     listenerPosition,
                                     listenerRotation,
                                     pitch,
                                     gain,
                                     rolloffFactor,
                                     buffers[buffer]);

                if (buffers[buffer].size() > result.size()) result.resize(buffers[buffer].size());

                for (uint32_t i = 0; i < buffers[buffer].size() && i < result.size(); ++i)
                {
                    // mix the sound into the buffer
                    result[i] += buffers[buffer][i];
                }
            }

            if (renderCommand.renderCallback)
            {
                if (!renderCommand.renderCallback(frames,
                                                  channels,
                                                  sampleRate,
                                                  listenerPosition,
                                                  listenerRotation,
                                                  pitch,
                                                  gain,
                                                  rolloffFactor,
                                                  result)) return false;
            }

            return true;
        }

        bool AudioDevice::getData(uint32_t frames, std::vector<uint8_t>& result)
        {
            currentBuffer = 0;
            uint32_t buffer = currentBuffer;

            if (++currentBuffer > buffers.size()) return true; // out of buffers

            buffers[buffer].resize(frames * channels);
            std::fill(buffers[buffer].begin(), buffers[buffer].end(), 0.0F);

            if (!processRenderCommands(frames, buffers[buffer])) return false;

            for (float& f : buffers[buffer])
            {
                f = clamp(f, -1.0F, 1.0F);
            }

            switch (sampleFormat)
            {
                case Audio::SampleFormat::SINT16:
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
                case Audio::SampleFormat::FLOAT32:
                {
                    result.reserve(frames * channels * sizeof(float));
                    result.assign(reinterpret_cast<uint8_t*>(buffers[buffer].data()),
                                  reinterpret_cast<uint8_t*>(buffers[buffer].data()) + buffers[buffer].size() * sizeof(float));
                    break;
                }
                default:
                    return false;
            }

            return true;
        }

        void AudioDevice::executeOnAudioThread(const std::function<void(void)>& func)
        {
            Lock lock(executeMutex);

            executeQueue.push(func);
        }

        void AudioDevice::executeAll()
        {
            std::function<void(void)> func;

            for (;;)
            {
                {
                    Lock lock(executeMutex);
                    if (executeQueue.empty()) break;

                    func = std::move(executeQueue.front());
                    executeQueue.pop();
                }

                if (func)
                {
                    func();
                }
            }
        }
    } // namespace audio
} // namespace ouzel
