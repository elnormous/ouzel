// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIO_HPP
#define OUZEL_AUDIO_AUDIO_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <vector>
#include "audio/Driver.hpp"
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
            Audio(Driver driver, bool debugAudio, Window* window);
            ~Audio();

            Audio(const Audio&) = delete;
            Audio& operator=(const Audio&) = delete;

            Audio(Audio&&) = delete;
            Audio& operator=(Audio&&) = delete;

            static std::set<Driver> getAvailableAudioDrivers();

            inline AudioDevice* getDevice() const { return device.get(); }

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

#endif // OUZEL_AUDIO_AUDIO_HPP
