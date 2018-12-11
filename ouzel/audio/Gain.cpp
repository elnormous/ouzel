// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cmath>
#include "Gain.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        class GainProcessor final: public Processor
        {
        public:
            GainProcessor()
            {
            }

            void process(uint32_t, uint16_t, uint32_t,
                         std::vector<float>& samples) override
            {
                for (float& sample : samples)
                    sample *= gainFactor;
            }

            void setGain(float newGain)
            {
                gain = newGain;
                gainFactor = powf(10.0F, gain / 20.0F);
            }

        private:
            float gain = 0.0F;
            float gainFactor = 1.0F;
        };

        Gain::Gain(Audio& initAudio):
            Filter(initAudio,
                   initAudio.initProcessor(std::unique_ptr<Processor>(new GainProcessor())))
        {
        }

        Gain::~Gain()
        {
        }

        void Gain::setGain(float newGain)
        {
            gain = newGain;

            audio.updateProcessor(processorId, [newGain](Object* node) {
                GainProcessor* gainProcessor = static_cast<GainProcessor*>(node);
                gainProcessor->setGain(newGain);
            });
        }
    } // namespace audio
} // namespace ouzel
