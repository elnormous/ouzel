// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Audio.h"
#include "SoundResource.h"
#include "math/MathUtils.h"

namespace ouzel
{
    namespace audio
    {
        Audio::Audio(Driver aDriver):
            driver(aDriver)
        {
        }

        Audio::~Audio()
        {
        }

        bool Audio::init(bool)
        {
            return true;
        }

        bool Audio::update()
        {
            executeAll();

            return true;
        }

        void Audio::deleteResource(SoundResource* resource)
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            std::vector<std::unique_ptr<SoundResource>>::iterator i = std::find_if(resources.begin(), resources.end(), [resource](const std::unique_ptr<SoundResource>& ptr) {
                return ptr.get() == resource;
            });

            if (i != resources.end())
            {
                resourceDeleteSet.push_back(std::move(*i));
                resources.erase(i);
            }
        }

        void Audio::setListenerPosition(const Vector3& newPosition)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            listenerPosition = newPosition;
        }

        void Audio::setListenerRotation(const Quaternion& newRotation)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            listenerRotation = newRotation;
        }

        bool Audio::getData(uint32_t samples, Format format, std::vector<uint8_t>& result)
        {
            buffer.resize(samples);
            std::fill(buffer.begin(), buffer.end(), 0.0f);

            {
                std::vector<float> data;

                std::lock_guard<std::mutex> lock(resourceMutex);

                for (const auto& resource : resources)
                {
                    if (!resource->getData(samples, channels, sampleRate, data))
                    {
                        return false;
                    }

                    for (uint32_t i = 0; i < data.size() && i < result.size(); ++i)
                    {
                        // mix the resource sound into the buffer
                        buffer[i] += data[i];
                    }
                }
            }

            for (float& f : buffer)
            {
                f = clamp(f, -1.0f, 1.0f);
            }

            switch (format)
            {
                case Format::SINT16:
                {
                    result.resize(samples * 2);
                    int16_t* resultPtr = reinterpret_cast<int16_t*>(result.data());

                    for (uint32_t i = 0; i < buffer.size(); ++i)
                    {
                        *resultPtr = static_cast<int16_t>(buffer[i] * 32767.0f);
                        ++resultPtr;
                    }
                    break;
                }
                case Format::FLOAT32:
                {
                    result.reserve(samples * 4);
                    result.assign(reinterpret_cast<uint8_t*>(buffer.data()),
                                  reinterpret_cast<uint8_t*>(buffer.data()) + buffer.size() * 4);
                    break;
                }
            }

            return true;
        }

        SoundResource* Audio::createSound()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            SoundResource* sound = new SoundResource();
            resources.push_back(std::unique_ptr<SoundResource>(sound));
            return sound;
        }

        void Audio::executeOnAudioThread(const std::function<void(void)>& func)
        {
            std::lock_guard<std::mutex> lock(executeMutex);

            executeQueue.push(func);
        }

        void Audio::executeAll()
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
