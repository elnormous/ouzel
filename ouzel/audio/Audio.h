// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <thread>
#include <memory>
#include <mutex>
#include <set>
#include <vector>
#include "math/Quaternion.h"
#include "math/Vector3.h"

#undef OPENAL

namespace ouzel
{
    class Engine;

    namespace audio
    {
        class Resource;
        class Sound;

        class Audio
        {
            friend Engine;
        public:
            const uint32_t SPEAKER_COUNT = 6;

            enum class Driver
            {
                DEFAULT,
                EMPTY,
                OPENAL,
                XAUDIO2,
                OPENSL
            };

            virtual ~Audio();

            virtual bool update();

            virtual std::shared_ptr<Sound> createSound() = 0;
            virtual void deleteResource(Resource* resource);

            virtual void setListenerPosition(const Vector3& newPosition);
            virtual void setListenerRotation(const Quaternion& newRotation);

            uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            void stop();

        protected:
            Audio(Driver aDriver);
            virtual bool init();
            void run();

            Driver driver;

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            Vector3 listenerPosition;
            Quaternion listenerRotation;

            std::mutex resourceMutex;
            std::vector<std::unique_ptr<Resource>> resources;
            std::vector<std::unique_ptr<Resource>> resourceDeleteSet;

            bool running = true;
#if OUZEL_MULTITHREADED
            std::thread audioThread;
#endif
        };
    } // namespace audio
} // namespace ouzel
