// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_PROCESSOR_HPP
#define OUZEL_AUDIO_MIXER_PROCESSOR_HPP

#include "audio/mixer/Object.hpp"
#include "audio/mixer/Bus.hpp"

namespace ouzel
{
    namespace audio
    {
        class Processor: public Object
        {
            friend Bus;
        public:
            Processor()
            {
            }
            ~Processor();

            Processor(const Processor&) = delete;
            Processor& operator=(const Processor&) = delete;

            Processor(Processor&&) = delete;
            Processor& operator=(Processor&&) = delete;

            void setBus(Bus* newBus);

            virtual void process(std::vector<float>& samples, uint16_t& channels,
                                 uint32_t& sampleRate, Vector3& position) = 0;

        private:
            Bus* bus = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_PROCESSOR_HPP
