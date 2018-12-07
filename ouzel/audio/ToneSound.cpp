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

            void getData(uint32_t frames, std::vector<float>& samples) override;
            
        private:
            uint32_t offset = 0;
        };

        class ToneData: public SourceData
        {
        public:
            ToneData(float initFrequency, ToneSound::Type initType, float initAmplitude):
                frequency(initFrequency),
                type(initType),
                amplitude(initAmplitude)
            {
                channels = 1;
                sampleRate = 44100;
            }

            float getFrequency() const { return frequency; }
            ToneSound::Type getType() const { return type; }
            float getAmplitude() const { return amplitude; }

            std::unique_ptr<Source> createSource() override
            {
                return std::unique_ptr<Source>(new ToneSource(*this));
            }

        private:
            float frequency;
            ToneSound::Type type;
            float amplitude;
        };

        ToneSource::ToneSource(ToneData& toneData):
            Source(toneData)
        {
        }

        void ToneSource::getData(uint32_t frames, std::vector<float>& samples)
        {
            ToneData& toneData = static_cast<ToneData&>(sourceData);

            samples.resize(frames);

            for (float& sample : samples)
            {
                switch (toneData.getType())
                {
                    case ToneSound::Type::SINE:
                        sample = sinf(offset * TAU * toneData.getFrequency() / toneData.getSampleRate());
                        break;
                    case ToneSound::Type::SQUARE:
                        sample = fmodf(roundf(offset * 2.0F * toneData.getFrequency() / toneData.getSampleRate() + 0.5F), 2.0F) * 2.0F - 1.0F;
                        break;
                    case ToneSound::Type::SAWTOOTH:
                        sample = fmodf(offset * toneData.getFrequency() / toneData.getSampleRate() + 0.5F, 1.0F) * 2.0F - 1.0F;
                        break;
                    case ToneSound::Type::TRIANGLE:
                        sample = fabsf(fmodf(offset * toneData.getFrequency() / toneData.getSampleRate() + 0.75F, 1.0F) * 2.0F - 1.0F) * 2.0F - 1.0F;
                        break;
                }

                sample *= toneData.getAmplitude();

                ++offset;
            }

            offset %= toneData.getSampleRate();
        }

        ToneSound::ToneSound(Audio& initAudio, float initFrequency, Type initType, float initAmplitude):
            Sound(initAudio, initAudio.initSourceData(std::unique_ptr<SourceData>(new ToneData(initFrequency, initType, initAmplitude)))),
            type(initType),
            frequency(initFrequency),
            amplitude(initAmplitude)
        {
        }
    } // namespace audio
} // namespace ouzel
