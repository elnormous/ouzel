// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Audio.h"
#include "SoundResource.h"

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
            running = false;

#if OUZEL_MULTITHREADED
            if (audioThread.joinable()) audioThread.join();
#endif
        }

        bool Audio::init()
        {
#if OUZEL_MULTITHREADED
            audioThread = std::thread(&Audio::run, this);
#endif

            return true;
        }

        bool Audio::update()
        {
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

            dirty |= DIRTY_LISTENER_POSITION;
        }

        void Audio::setListenerRotation(const Quaternion& newRotation)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            listenerRotation = newRotation;

            dirty |= DIRTY_LISTENER_ROTATION;
        }

        void Audio::run()
        {
            while (running)
            {
                update();

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        void Audio::stop()
        {
            running = false;

#if OUZEL_MULTITHREADED
            if (audioThread.joinable()) audioThread.join();
#endif
        }

        std::vector<uint8_t> Audio::getData(uint32_t size)
        {
            std::vector<uint8_t> data(size, 0);

            std::lock_guard<std::mutex> lock(resourceMutex);

            for (const auto& resource : resources)
            {
                std::vector<uint8_t> resourceData = resource->getData(bufferSize, channels, samplesPerSecond);

                int16_t* resourceDataPtr = reinterpret_cast<int16_t*>(resourceData.data());
                int16_t* dataPtr = reinterpret_cast<int16_t*>(data.data());

                for (uint32_t i = 0; i < resourceData.size() / sizeof(int16_t) && i < data.size() / sizeof(int16_t); ++i)
                {
                    // mix the resource sound into the buffer
                    *dataPtr += *resourceDataPtr;

                    ++resourceDataPtr;
                    ++dataPtr;
                }
            }

            return data;
        }
    } // namespace audio
} // namespace ouzel
