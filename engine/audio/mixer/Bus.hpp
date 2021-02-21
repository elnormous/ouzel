// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_BUS_HPP
#define OUZEL_AUDIO_MIXER_BUS_HPP

#include <vector>
#include "Object.hpp"

namespace ouzel::audio::mixer
{
    class Processor;
    class Stream;

    class Bus final: public Object
    {
        friend Processor;
        friend Stream;
    public:
        Bus() noexcept {}
        ~Bus() override;
        Bus(const Bus&) = delete;
        Bus& operator=(const Bus&) = delete;

        Bus(Bus&&) = delete;
        Bus& operator=(Bus&&) = delete;

        void setOutput(Bus* newOutput);

        void generateSamples(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate,
                             const Vector3F& listenerPosition, const QuaternionF& listenerRotation,
                             std::vector<float>& samples);

        void addProcessor(Processor* processor);
        void removeProcessor(Processor* processor);

    private:
        void addInput(Bus* bus);
        void removeInput(Bus* bus);
        void addInput(Stream* stream);
        void removeInput(Stream* stream);

        Bus* output = nullptr;
        std::vector<Bus*> inputBuses;
        std::vector<Stream*> inputStreams;
        std::vector<Processor*> processors;

        std::vector<float> resampleBuffer;
        std::vector<float> mixBuffer;
        std::vector<float> buffer;
    };
}

#endif // OUZEL_AUDIO_MIXER_BUS_HPP
