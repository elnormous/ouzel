// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_BUS_HPP
#define OUZEL_AUDIO_MIXER_BUS_HPP

#include <vector>
#include "audio/mixer/Object.hpp"
#include "audio/mixer/Processor.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Source;

            class Bus final: public Object
            {
            public:
                Bus() noexcept {}
                explicit Bus(std::unique_ptr<Processor> initProcessor) noexcept:
                    processor(std::move(initProcessor))
                {
                }
                Bus(const Bus&) = delete;
                Bus& operator=(const Bus&) = delete;

                Bus(Bus&&) = delete;
                Bus& operator=(Bus&&) = delete;

                void addInput(Bus* bus);
                void removeInput(Bus* bus);
                void addInput(Source* stream);
                void removeInput(Source* stream);

                void getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                                std::vector<float>& samples) final;

            private:
                std::unique_ptr<Processor> processor;

                Bus* output = nullptr;
                std::vector<Bus*> inputBuses;
                std::vector<Source*> inputSources;

                std::vector<float> resampleBuffer;
                std::vector<float> mixBuffer;
                std::vector<float> buffer;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_BUS_HPP
