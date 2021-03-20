// Ouzel by Elviss Strazdins

#include <iterator>
#include <stdexcept>
#include "PcmClip.hpp"
#include "Audio.hpp"
#include "mixer/Data.hpp"
#include "mixer/Stream.hpp"

namespace ouzel::audio
{
    class PcmData;

    class PcmStream final: public mixer::Stream
    {
    public:
        explicit PcmStream(PcmData& pcmData);

        void reset() final
        {
            position = 0;
        }

        void generateSamples(std::uint32_t frames, std::vector<float>& samples) final;

    private:
        std::uint32_t position = 0;
    };

    class PcmData final: public mixer::Data
    {
    public:
        PcmData(std::uint32_t initChannels, std::uint32_t initSampleRate,
                const std::vector<float>& initData):
            Data(initChannels, initSampleRate),
            data(initData)
        {
        }

        auto& getData() const noexcept { return data; }

        std::unique_ptr<mixer::Stream> createStream() final
        {
            return std::make_unique<PcmStream>(*this);
        }

    private:
        std::vector<float> data;
    };

    PcmStream::PcmStream(PcmData& pcmData):
        Stream(pcmData)
    {
    }

    void PcmStream::generateSamples(std::uint32_t frames, std::vector<float>& samples)
    {
        const std::uint32_t neededSize = frames * data.getChannels();
        samples.resize(neededSize);

        const auto& pcmData = static_cast<PcmData&>(data);
        const std::vector<float>& dataSamples = pcmData.getData();

        const auto sourceFrames = static_cast<std::uint32_t>(dataSamples.size() / pcmData.getChannels());
        const std::uint32_t copyFrames = (frames > sourceFrames - position) ? sourceFrames - position : frames;

        for (std::uint32_t channel = 0; channel < pcmData.getChannels(); ++channel)
        {
            const auto sourceChannel = &dataSamples[channel * sourceFrames];
            auto outputChannel = &samples[channel * frames];

            for (std::uint32_t frame = 0; frame < copyFrames; ++frame)
                outputChannel[frame] = sourceChannel[frame + position];
        }

        position += copyFrames;

        for (std::uint32_t channel = 0; channel < pcmData.getChannels(); ++channel)
        {
            auto outputChannel = &samples[channel * frames];

            for (std::uint32_t frame = copyFrames; frame < frames; ++frame)
                outputChannel[frame] = 0.0F;
        }

        if ((sourceFrames - position) == 0)
        {
            playing = false; // TODO: fire event
            reset();
        }
    }

    PcmClip::PcmClip(Audio& initAudio, std::uint32_t channels, std::uint32_t sampleRate,
                      const std::vector<float>& samples):
        Sound(initAudio,
              initAudio.initData(std::unique_ptr<mixer::Data>(data = new PcmData(channels, sampleRate, samples))),
              Sound::Format::pcm)
    {
    }
}
