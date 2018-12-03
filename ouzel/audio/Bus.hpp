// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_BUS_HPP
#define OUZEL_AUDIO_BUS_HPP

#include "audio/Object.hpp"

namespace ouzel
{
    namespace audio
    {
        class Processor;

        class Bus final: public Object
        {
        public:
            Bus();
            Bus(const Bus&) = delete;
            Bus& operator=(const Bus&) = delete;

            Bus(Bus&&) = delete;
            Bus& operator=(Bus&&) = delete;

            void setOutput(Bus* newOutput);
            
            void addProcessor(Processor* processor);
            void removeProcessor(Processor* processor);

        private:
            Bus* output = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif //OUZEL_AUDIO_BUS_HPP
