// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "AudioDevice.hpp"
#include "SoundResource.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        AudioDevice::AudioDevice(Audio::Driver aDriver):
            driver(aDriver)
        {
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

            std::lock_guard<std::mutex> renderQueueLock(renderQueueMutex);

            std::vector<float> data;

            for (const RenderCommand& renderCommand : renderQueue)
            {
                if (!processRenderCommand(renderCommand, data)) return false;
            }

            return true;
        }

        bool AudioDevice::processRenderCommand(const RenderCommand& renderCommand, std::vector<float>& result)
        {
            std::vector<float> data;

            for (const RenderCommand& renderCommand : renderQueue)
            {
                processRenderCommand(renderCommand, result);
            }

            return true;
        }

        void AudioDevice::deleteResource(Resource* resource)
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            auto resourceIterator = std::find_if(resources.begin(), resources.end(), [resource](const std::unique_ptr<Resource>& ptr) {
                return ptr.get() == resource;
            });

            if (resourceIterator != resources.end())
            {
                resourceDeleteSet.push_back(std::move(*resourceIterator));
                resources.erase(resourceIterator);
            }
        }

        bool AudioDevice::getData(uint32_t frames, std::vector<uint8_t>& result)
        {
            buffer.resize(frames * channels);
            std::fill(buffer.begin(), buffer.end(), 0.0f);

            {
                std::vector<float> data;

                std::lock_guard<std::mutex> lock(resourceMutex);

                /*for (ListenerResource* listener : listeners)
                {
                    if (!listener->getData(frames, channels, sampleRate, data))
                    {
                        return false;
                    }

                    for (uint32_t i = 0; i < data.size() && i < result.size(); ++i)
                    {
                        // mix the resource sound into the buffer
                        buffer[i] += data[i];
                    }
                }*/
            }

            for (float& f : buffer)
            {
                f = clamp(f, -1.0f, 1.0f);
            }

            switch (format)
            {
                case Audio::Format::SINT16:
                {
                    result.resize(frames * channels * sizeof(int16_t));
                    int16_t* resultPtr = reinterpret_cast<int16_t*>(result.data());

                    for (uint32_t i = 0; i < buffer.size(); ++i)
                    {
                        *resultPtr = static_cast<int16_t>(buffer[i] * 32767.0f);
                        ++resultPtr;
                    }
                    break;
                }
                case Audio::Format::FLOAT32:
                {
                    result.reserve(frames * channels * sizeof(float));
                    result.assign(reinterpret_cast<uint8_t*>(buffer.data()),
                                  reinterpret_cast<uint8_t*>(buffer.data()) + buffer.size() * 4);
                    break;
                }
            }

            return true;
        }

        SoundResource* AudioDevice::createSound()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            SoundResource* sound = new SoundResource(this);
            resources.push_back(std::unique_ptr<Resource>(sound));
            return sound;
        }

        void AudioDevice::executeOnAudioThread(const std::function<void(void)>& func)
        {
            std::lock_guard<std::mutex> lock(executeMutex);

            executeQueue.push(func);
        }

        void AudioDevice::executeAll()
        {
            std::function<void(void)> func;

            for (;;)
            {
                {
                    std::lock_guard<std::mutex> lock(executeMutex);

                    if (executeQueue.empty())
                    {
                        break;
                    }
                    
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
