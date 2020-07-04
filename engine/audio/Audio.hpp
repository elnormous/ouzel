// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIO_HPP
#define OUZEL_AUDIO_AUDIO_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <vector>
#include "AudioDevice.hpp"
#include "Driver.hpp"
#include "Mix.hpp"
#include "Node.hpp"
#include "mixer/Commands.hpp"
#include "mixer/Processor.hpp"
#include "mixer/Mixer.hpp"
#include "../math/Quaternion.hpp"
#include "../math/Vector.hpp"

namespace ouzel::audio
{
    class Listener;

    class Audio final
    {
    public:
        Audio(Driver driver, bool debugAudio);

        static Driver getDriver(const std::string& driver);
        static std::set<Driver> getAvailableAudioDrivers();

        auto getDevice() const noexcept { return device.get(); }
        mixer::Mixer& getMixer() { return mixer; }
        Mix& getMasterMix() { return masterMix; }

        void update();

        void addCommand(std::unique_ptr<mixer::Command> command)
        {
            commandBuffer.pushCommand(std::move(command));
        }

        void deleteObject(std::size_t objectId);
        std::size_t initBus();
        std::size_t initStream(std::size_t sourceId);
        std::size_t initData(std::unique_ptr<mixer::Data> data);
        std::size_t initProcessor(std::unique_ptr<mixer::Processor> processor);
        void updateProcessor(std::size_t processorId, const std::function<void(mixer::Processor*)>& updateFunction);

        auto& getRootNode() { return rootNode; }

    private:
        void getSamples(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate, std::vector<float>& samples);
        void eventCallback(const mixer::Mixer::Event& event);

        std::unique_ptr<AudioDevice> device;
        mixer::Mixer mixer;
        mixer::CommandBuffer commandBuffer;
        Mix masterMix;
        Node rootNode;
    };
}

#endif // OUZEL_AUDIO_AUDIO_HPP
