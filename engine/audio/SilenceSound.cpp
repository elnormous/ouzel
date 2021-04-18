// Ouzel by Elviss Strazdins

#include "SilenceSound.hpp"
#include "Audio.hpp"
#include "mixer/Data.hpp"
#include "mixer/Stream.hpp"

namespace ouzel::audio
{
    class SilenceData;

    class SilenceStream final: public mixer::Stream
    {
    public:
        explicit SilenceStream(SilenceData& toneData);

        void reset() override
        {
            position = 0;
        }

        void generateSamples(std::uint32_t frames, std::vector<float>& samples) override;

    private:
        std::uint32_t position = 0;
    };

    class SilenceData final: public mixer::Data
    {
    public:
        explicit SilenceData(float initLength):
            Data{1, 44100},
            length{initLength}
        {
        }

        auto getLength() const noexcept { return length; }

        std::unique_ptr<mixer::Stream> createStream() override
        {
            return std::make_unique<SilenceStream>(*this);
        }

    private:
        float length;
    };

    SilenceStream::SilenceStream(SilenceData& silenceData):
        Stream{silenceData}
    {
    }

    void SilenceStream::generateSamples(std::uint32_t frames, std::vector<float>& samples)
    {
        auto& silenceData = static_cast<SilenceData&>(data);

        samples.resize(frames);
        std::fill(samples.begin(), samples.end(), 0.0F); // TODO: fill only the needed samples

        const auto length = static_cast<SilenceData&>(silenceData).getLength();

        if (length > 0.0F)
        {
            const auto frameCount = static_cast<std::uint32_t>(length * data.getSampleRate());

            if (auto neededSize = frames; neededSize > 0)
            {
                if (frameCount - position < neededSize)
                {
                    neededSize -= frameCount - position;
                    position = frameCount;
                }
                else
                {
                    position += neededSize;
                    neededSize = 0;
                }
            }

            if ((frameCount - position) == 0)
            {
                playing = false; // TODO: fire event
                reset();
            }
        }
        else
        {
            position += frames;
        }
    }

    SilenceSound::SilenceSound(Audio& initAudio, float initLength):
        Sound{
            initAudio,
            initAudio.initData(std::unique_ptr<mixer::Data>(data = new SilenceData(initLength))),
            Sound::Format::pcm
        },
        length{initLength}
    {
    }
}
