// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIO_HPP
#define OUZEL_AUDIO_AUDIO_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <vector>
#include "audio/Driver.hpp"
#include "audio/Object.hpp"
#include "audio/Bus.hpp"
#include "audio/Mixer.hpp"
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
            inline Mixer& getMixer() { return mixer; }

            void update();

            uintptr_t initObject(const std::function<std::unique_ptr<Object>(void)>& createFunction);
            void deleteObject(uintptr_t objectId);
            void updateObject(uintptr_t objectId, const std::function<void(Object*)>& updateFunction);

            Bus& getMasterBus() { return masterBus; }

        private:
            std::unique_ptr<AudioDevice> device;
            Mixer mixer;
            Bus masterBus;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_AUDIO_HPP
