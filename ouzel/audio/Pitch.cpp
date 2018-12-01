// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Pitch.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        class PitchProcessor final: public Object
        {
        public:
            PitchProcessor()
            {
            }

            void process(std::vector<float>& samples, uint16_t& channels,
                         uint32_t& sampleRate, Vector3& position) override
            {
                Object::process(samples, channels, sampleRate, position);
            }
        };

        Pitch::Pitch(Audio& initAudio):
            Filter(initAudio),
            objectId(audio.initObject([]() { return std::unique_ptr<Object>(new PitchProcessor()); }))
        {
        }

        Pitch::~Pitch()
        {
            if (objectId) audio.deleteObject(objectId);
        }

        void Pitch::setPitch(float newPitch)
        {
            pitch = newPitch;
        }
    } // namespace audio
} // namespace ouzel
