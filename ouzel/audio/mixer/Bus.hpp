// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_BUS_HPP
#define OUZEL_AUDIO_MIXER_BUS_HPP

#include <vector>
#include "audio/mixer/Object.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Processor;
            class Stream;

            class Bus final: public Object
            {
                friend Processor;
                friend Stream;
            public:
                Bus() noexcept {}
                ~Bus();
                Bus(const Bus&) = delete;
                Bus& operator=(const Bus&) = delete;

                Bus(Bus&&) = delete;
                Bus& operator=(Bus&&) = delete;

                void setOutput(Bus* newOutput);

                void getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples) final
                {
                }

                void getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate,
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
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_BUS_HPP
