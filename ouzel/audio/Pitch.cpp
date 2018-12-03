// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Pitch.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        class PitchProcessor final: public Processor
        {
        public:
            PitchProcessor()
            {
            }

            void process(std::vector<float>& samples, uint16_t& channels,
                         uint32_t& sampleRate, Vector3& position) override
            {
            }
        };

        Pitch::Pitch(Audio& initAudio):
            Filter(initAudio,
                   initAudio.initProcessor([]() { return std::unique_ptr<Processor>(new PitchProcessor()); }))
        {
        }

        Pitch::~Pitch()
        {
        }

        void Pitch::setPitch(float newPitch)
        {
            pitch = newPitch;
        }
    } // namespace audio
} // namespace ouzel
