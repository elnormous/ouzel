// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Filter.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        class FilterProcessor final: public Processor
        {
        public:
            FilterProcessor()
            {
            }

            void process(std::vector<float>& samples, uint16_t& channels,
                         uint32_t& sampleRate, Vector3& position) override
            {
                Object::process(samples, channels, sampleRate, position);
            }
        };

        Filter::Filter(Audio& initAudio):
            audio(initAudio),
            objectId(audio.initObject([]() { return std::unique_ptr<Processor>(new FilterProcessor()); }))
        {
        }

        Filter::~Filter()
        {
            if (objectId) audio.deleteObject(objectId);
        }
    } // namespace audio
} // namespace ouzel
