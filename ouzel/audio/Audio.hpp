// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <vector>
#include "math/Quaternion.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    class Engine;

    namespace audio
    {
        class AudioDevice;

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
                DIRECTSOUND,
                XAUDIO2,
                OPENSL,
                COREAUDIO,
                ALSA
            };

            enum Channel
            {
                CHANNEL_FRONT_LEFT = 0,
                CHANNEL_FRONT_RIGHT = 1,
                CHANNEL_CENTER = 2,
                CHANNEL_LFE = 3,
                CHANNEL_BACK_LEFT = 4,
                CHANNEL_BACK_RIGHT = 5,
                CHANNEL_SIDE_LEFT = 6,
                CHANNEL_SIDE_RIGHT = 7
            };

            enum class Format
            {
                SINT16,
                FLOAT32
            };

            AudioDevice* getDevice() const { return device.get(); }

            static std::set<Audio::Driver> getAvailableAudioDrivers();

            bool update();

            const Vector3& getListenerPosition() const { return listenerPosition; }
            void setListenerPosition(const Vector3& newPosition);

            const Quaternion& getListenerRotation() const { return listenerRotation; }
            void setListenerRotation(const Quaternion& newRotation);

            void executeOnAudioThread(const std::function<void(void)>& func);

        protected:
            Audio(Driver driver);
            bool init(bool debugAudio);

            std::unique_ptr<AudioDevice> device;

            Vector3 listenerPosition;
            Quaternion listenerRotation;
        };
    } // namespace audio
} // namespace ouzel
