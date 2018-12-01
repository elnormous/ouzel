// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIO_HPP
#define OUZEL_AUDIO_AUDIO_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <vector>
#include "audio/Driver.hpp"
#include "audio/Node.hpp"
#include "audio/Bus.hpp"
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

            void update();

            uintptr_t initNode(const std::function<std::unique_ptr<Node>(void)>& createFunction);
            void deleteNode(uintptr_t nodeId);
            void updateNode(uintptr_t nodeId, const std::function<void(Node*)>& updateFunction);

            Bus& getMasterBus() { return masterBus; }

        private:
            std::unique_ptr<AudioDevice> device;
            Bus masterBus;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_AUDIO_HPP
