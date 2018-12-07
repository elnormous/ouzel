// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
                         Vector3, std::vector<float>& samples) override
            {
                for (float& sample : samples)
                    sample *= gain;
            }

            void setGain(float newGain)
            {
                gain = newGain;
            }

        private:
            float gain = 1.0F;
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
