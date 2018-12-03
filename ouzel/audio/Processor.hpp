// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_PROCESSOR_HPP
#define OUZEL_AUDIO_PROCESSOR_HPP

#include "audio/Object.hpp"
#include "audio/Bus.hpp"

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

            ~Processor()
            {
                if (bus) bus->removeProcessor(this);
            }

            Processor(const Processor&) = delete;
            Processor& operator=(const Processor&) = delete;

            Processor(Processor&&) = delete;
            Processor& operator=(Processor&&) = delete;

            virtual void process(std::vector<float>& samples, uint16_t& channels,
                                 uint32_t& sampleRate, Vector3& position) = 0;

        private:
            Bus* bus = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_PROCESSOR_HPP
