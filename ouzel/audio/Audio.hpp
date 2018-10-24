// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <vector>
#include "math/Quaternion.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    class Window;

    namespace audio
    {
        class AudioDevice;
        class Listener;

        class Audio final
        {
        public:
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

            enum class Channel
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

            enum class ChannelConfiguration
            {
                MONO,
                STEREO,
                QUAD,
                SURROUND51,
                SURROUND51_REAR,
                SURROUND61,
                SURROUND71
            };

            enum class SampleFormat
            {
                SINT16,
                FLOAT32
            };

            Audio(Driver driver, bool debugAudio, Window* window);
            ~Audio();

            Audio(const Audio&) = delete;
            Audio& operator=(const Audio&) = delete;

            Audio(Audio&&) = delete;
            Audio& operator=(Audio&&) = delete;

            static std::set<Audio::Driver> getAvailableAudioDrivers();

            inline AudioDevice* getDevice() const { return device.get(); }

            void update();

            void addListener(Listener* listener);
            void removeListener(Listener* listener);

            static void resample(const std::vector<float>& src, uint32_t srcFrames,
                                 std::vector<float>& dst, uint32_t dstFrames,
                                 uint32_t channels);

        private:
            std::unique_ptr<AudioDevice> device;

            std::vector<Listener*> listeners;
        };
    } // namespace audio
} // namespace ouzel
