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

            void getData(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                         const Vector3& listenerPosition, const Quaternion& listenerRotation,
                         std::vector<float>& samples);

            void addProcessor(Processor* processor);
            void removeProcessor(Processor* processor);

        private:
            void addInput(Bus* bus);
            void removeInput(Bus* bus);
            void addInput(Source* source);
            void removeInput(Source* source);

            Bus* output = nullptr;
            std::vector<Bus*> inputBuses;
            std::vector<Source*> inputSources;
            std::vector<Processor*> processors;

            std::vector<float> resampleBuffer;
            std::vector<float> mixBuffer;
            std::vector<float> buffer;
        };
    } // namespace audio
} // namespace ouzel

#endif //OUZEL_AUDIO_MIXER_BUS_HPP
