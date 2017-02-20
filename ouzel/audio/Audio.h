// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <set>
#include <vector>
#include "math/Quaternion.h"
#include "math/Vector3.h"
#include "utils/Types.h"

#undef OPENAL

namespace ouzel
{
    class Engine;

    namespace audio
    {
        class Resource;

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

            virtual bool process();

            virtual void setListenerPosition(const Vector3& newPosition);
            virtual void setListenerRotation(const Quaternion& newRotation);

            virtual SoundPtr createSound() = 0;

            uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            bool isReady() const { return ready; }

        protected:
            Audio(Driver aDriver);
            virtual bool init();

            Driver driver;

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;

            bool ready = false;

            Vector3 listenerPosition;
            Quaternion listenerRotation;

            std::mutex resourceMutex;
            std::vector<std::unique_ptr<Resource>> resources;
            std::set<Resource*> resourceUploadSet;
            std::vector<std::unique_ptr<Resource>> resourceDeleteSet;
        };
    } // namespace audio
} // namespace ouzel
