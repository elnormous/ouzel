// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_BUS_HPP
#define OUZEL_AUDIO_MIXER_BUS_HPP

#include <vector>
#include "audio/mixer/Object.hpp"

namespace ouzel
{
    namespace audio
    {
        class Processor;
        class Source;

        class Bus final: public Object
        {
            friend Processor;
            friend Source;
        public:
            Bus();
            ~Bus();
            Bus(const Bus&) = delete;
            Bus& operator=(const Bus&) = delete;

            Bus(Bus&&) = delete;
            Bus& operator=(Bus&&) = delete;

            void setOutput(Bus* newOutput);

            void getData(std::vector<float>& samples, uint16_t& channels,
                         uint32_t& sampleRate, Vector3& position);

        private:
            void addInput(Bus* bus);
            void removeInput(Bus* bus);
            void addInput(Source* source);
            void removeInput(Source* source);
            void addProcessor(Processor* processor);
            void removeProcessor(Processor* processor);

            Bus* output = nullptr;
            std::vector<Bus*> inputBuses;
            std::vector<Source*> inputSources;
            std::vector<Processor*> processors;
        };
    } // namespace audio
} // namespace ouzel

#endif //OUZEL_AUDIO_MIXER_BUS_HPP
