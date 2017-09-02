// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <set>
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
                FRONT_LEFT = 0,
                FRONT_RIGHT = 1,
                CENTER = 2,
                LFE = 3,
                BACK_LEFT = 4,
                BACK_RIGHT = 5,
                SIDE_LEFT = 6,
                SIDE_RIGHT = 7
            };

            enum ChannelConfiguration
            {
                MONO,
                STEREO,
                QUAD,
                SURROUND51,
                SURROUND51_REAR,
                SURROUND61,
                SURROUND71
            };

            enum class Format
            {
                SINT16,
                FLOAT32
            };

            ~Audio();

            static std::set<Audio::Driver> getAvailableAudioDrivers();

            AudioDevice* getDevice() const { return device.get(); }

            bool update();

            void executeOnAudioThread(const std::function<void(void)>& func);

            const Vector3& getListenerPosition() const { return listenerPosition; }
            void setListenerPosition(const Vector3& newPosition);

            const Quaternion& getListenerRotation() const { return listenerRotation; }
            void setListenerRotation(const Quaternion& newRotation);

        protected:
            Audio(Driver driver);
            bool init(bool debugAudio);

            std::unique_ptr<AudioDevice> device;

            Vector3 listenerPosition;
            Quaternion listenerRotation;
        };
    } // namespace audio
} // namespace ouzel
