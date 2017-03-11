// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

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
            std::lock_guard<std::mutex> lock(resourceMutex);

            for (const std::unique_ptr<Resource>& resource : resources)
            {
                resource->update();
            }

            return true;
        }

        void Audio::deleteResource(Resource* resource)
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            std::vector<std::unique_ptr<Resource>>::iterator i = std::find_if(resources.begin(), resources.end(), [resource](const std::unique_ptr<Resource>& ptr) {
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
            listenerPosition = newPosition;
        }

        void Audio::setListenerRotation(const Quaternion& newRotation)
        {
            listenerRotation = newRotation;
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
    } // namespace audio
} // namespace ouzel
