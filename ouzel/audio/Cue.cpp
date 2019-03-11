// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Cue.hpp"
#include "Audio.hpp"
#include "mixer/Stream.hpp"
#include "mixer/Source.hpp"

namespace ouzel
{
    namespace audio
    {
        class CueData;

        class CueSource final: public mixer::Stream
        {
        public:
            CueSource(CueData& pcmData);

            void reset() override
            {
                position = 0;
            }

            void getData(uint32_t frames, std::vector<float>& samples) override;

        private:
            uint32_t position = 0;
        };

        class CueData final: public mixer::Source
        {
        public:
            CueData()
            {
            }

            std::unique_ptr<mixer::Stream> createStream() override
            {
                return std::unique_ptr<mixer::Stream>(new CueSource(*this));
            }
        };

        CueSource::CueSource(CueData& pcmData):
            Stream(pcmData)
        {
        }

        void CueSource::getData(uint32_t frames, std::vector<float>& samples)
        {
        }

        Cue::Cue(Audio& initAudio):
            Sound(initAudio, initAudio.initSource([](){
                return std::unique_ptr<mixer::Source>(new CueData());
            }))
        {

        }
    } // namespace audio
} // namespace ouzel
