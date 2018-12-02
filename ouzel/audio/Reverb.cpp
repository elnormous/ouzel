// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Reverb.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        class ReverbProcessor final: public Object
        {
        public:
            ReverbProcessor()
            {
            }

            void process(std::vector<float>& samples, uint16_t& channels,
                         uint32_t& sampleRate, Vector3& position) override
            {
                Object::process(samples, channels, sampleRate, position);
            }
        };

        Reverb::Reverb(Audio& initAudio):
            Filter(initAudio),
            objectId(audio.initObject([]() { return std::unique_ptr<Object>(new ReverbProcessor()); }))
        {
        }

        Reverb::~Reverb()
        {
            if (objectId) audio.deleteObject(objectId);
        }
    } // namespace audio
} // namespace ouzel

