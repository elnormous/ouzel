// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "ToneSound.hpp"
#include "Audio.hpp"
#include "mixer/Source.hpp"
#include "mixer/SourceData.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        class ToneData;

        class ToneSource: public Source
        {
        public:
            ToneSource(ToneData& toneData);

            void reset() override
            {
            }

        private:
            void getData(std::vector<float>& samples) override;
            uint32_t offset = 0;
        };

        class ToneData: public SourceData
        {
        public:
            ToneData(float initFrequency, float initAmplitude):
                frequency(initFrequency),
                amplitude(initAmplitude)
            {
                channels = 1;
                sampleRate = 44100;
            }

            float getFrequency() const { return frequency; }
            float getAmplitude() const { return amplitude; }

            std::unique_ptr<Source> createSource() override
            {
                return std::unique_ptr<Source>(new ToneSource(*this));
            }

        private:
            float frequency;
            float amplitude;
        };

        ToneSource::ToneSource(ToneData& toneData):
            Source(toneData)
        {
        }

        void ToneSource::getData(std::vector<float>& samples)
        {
            ToneData& toneData = static_cast<ToneData&>(sourceData);

            for (float& sample : samples)
            {
                sample = sinf(TAU * toneData.getFrequency() * static_cast<float>(offset) / toneData.getSampleRate()) * toneData.getAmplitude();
                ++offset;
            }

            offset %= toneData.getSampleRate();
        }

        // TODO: implement wave shape
        ToneSound::ToneSound(Audio& initAudio, float initFrequency, float initAmplitude):
            Sound(initAudio, initAudio.initSourceData(std::unique_ptr<SourceData>(new ToneData(initFrequency, initAmplitude)))),
            frequency(initFrequency),
            amplitude(initAmplitude)
        {
        }
    } // namespace audio
} // namespace ouzel
