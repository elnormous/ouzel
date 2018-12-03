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

        class Bus final: public Object
        {
        public:
            Bus();
            ~Bus();
            Bus(const Bus&) = delete;
            Bus& operator=(const Bus&) = delete;

            Bus(Bus&&) = delete;
            Bus& operator=(Bus&&) = delete;

            void setOutput(Bus* newOutput);
            
            void addProcessor(Processor* processor);
            void removeProcessor(Processor* processor);

            void getData(std::vector<float>& samples, uint16_t& channels,
                         uint32_t& sampleRate, Vector3& position);
        private:
            Bus* output = nullptr;
            std::vector<Processor*> processors;
        };
    } // namespace audio
} // namespace ouzel

#endif //OUZEL_AUDIO_MIXER_BUS_HPP
