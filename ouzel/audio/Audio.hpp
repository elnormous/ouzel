// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIO_HPP
#define OUZEL_AUDIO_AUDIO_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <vector>
#include "audio/Driver.hpp"
#include "audio/Mix.hpp"
#include "audio/mixer/Processor.hpp"
#include "audio/mixer/Mixer.hpp"
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

            void deleteObject(uintptr_t objectId);
            uintptr_t initBus();
            uintptr_t initProcessor(const std::function<std::unique_ptr<Processor>(void)>& createFunction);
            void updateProcessor(uintptr_t processorId, const std::function<void(Processor*)>& updateFunction);

            Mix& getMasterMix() { return masterMix; }

        private:
            std::unique_ptr<AudioDevice> device;
            Mixer mixer;
            Mix masterMix;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_AUDIO_HPP
