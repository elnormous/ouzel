// Ouzel by Elviss Strazdins

#include <stdexcept>
#include "VorbisClip.hpp"
#include "Audio.hpp"
#include "mixer/Data.hpp"
#include "mixer/Stream.hpp"
#include "../utils/Utils.hpp"

#ifdef _MSC_VER
#  pragma warning( push )
#  pragma warning( disable : 4100 )
#  pragma warning( disable : 4244 )
#  pragma warning( disable : 4245 )
#  pragma warning( disable : 4456 )
#  pragma warning( disable : 4457 )
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wdouble-promotion"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wtype-limits"
#  pragma GCC diagnostic ignored "-Wunused-function"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wunused-value"
#  ifdef __clang__
#    pragma GCC diagnostic ignored "-Wcomma"
#    pragma GCC diagnostic ignored "-Wconditional-uninitialized"
#  else
#    pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#  endif
#endif

#include "stb_vorbis.c"

#ifdef _MSC_VER
#  pragma warning( pop )
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

namespace ouzel::audio
{
    class VorbisData;

    class VorbisStream final: public mixer::Stream
    {
    public:
        explicit VorbisStream(VorbisData& vorbisData);

        ~VorbisStream() override
        {
            if (vorbisStream)
                stb_vorbis_close(vorbisStream);
        }

        void reset() override
        {
            stb_vorbis_seek_start(vorbisStream);
        }

        void generateSamples(std::uint32_t frames, std::vector<float>& samples) override;

    private:
        stb_vorbis* vorbisStream = nullptr;
    };

    class VorbisData final: public mixer::Data
    {
    public:
        explicit VorbisData(const std::vector<std::byte>& initData):
            data{initData}
        {
            stb_vorbis* vorbisStream = stb_vorbis_open_memory(reinterpret_cast<const unsigned char*>(data.data()),
                                                              static_cast<int>(data.size()),
                                                              nullptr, nullptr);

            if (!vorbisStream)
                throw std::runtime_error{"Failed to load Vorbis stream"};

            stb_vorbis_info info = stb_vorbis_get_info(vorbisStream);

            channels = static_cast<std::uint32_t>(info.channels);
            sampleRate = info.sample_rate;

            stb_vorbis_close(vorbisStream);
        }

        auto& getData() const noexcept { return data; }

        std::unique_ptr<mixer::Stream> createStream() override
        {
            return std::make_unique<VorbisStream>(*this);
        }

    private:
        std::vector<std::byte> data;
    };

    VorbisStream::VorbisStream(VorbisData& vorbisData):
        Stream{vorbisData}
    {
        vorbisStream = stb_vorbis_open_memory(reinterpret_cast<const unsigned char*>(vorbisData.getData().data()),
                                              static_cast<int>(vorbisData.getData().size()),
                                              nullptr, nullptr);
    }

    void VorbisStream::generateSamples(std::uint32_t frames, std::vector<float>& samples)
    {
        std::uint32_t neededSize = frames * data.getChannels();
        samples.resize(neededSize);

        int resultFrames = 0;

        if (neededSize > 0)
        {
            if (vorbisStream->eof)
                reset();

            std::vector<float*> channelData(data.getChannels());

            switch (data.getChannels())
            {
                case 1:
                    channelData[0] = &samples[0];
                    break;
                case 2:
                    channelData[0] = &samples[0 * frames];
                    channelData[1] = &samples[1 * frames];
                    break;
                case 4:
                    channelData[0] = &samples[0 * frames];
                    channelData[1] = &samples[1 * frames];
                    channelData[2] = &samples[2 * frames];
                    channelData[3] = &samples[3 * frames];
                    break;
                case 6:
                    channelData[0] = &samples[0 * frames];
                    channelData[1] = &samples[2 * frames];
                    channelData[2] = &samples[1 * frames];
                    channelData[3] = &samples[4 * frames];
                    channelData[4] = &samples[5 * frames];
                    channelData[5] = &samples[3 * frames];
                    break;
                default:
                    throw std::runtime_error{"Unsupported channel count"};
            }

            resultFrames = stb_vorbis_get_samples_float(vorbisStream,
                                                        static_cast<int>(data.getChannels()),
                                                        channelData.data(),
                                                        static_cast<int>(frames));
        }

        if (vorbisStream->eof)
        {
            playing = false; // TODO: fire event
            reset();
        }

        for (std::uint32_t channel = 0; channel < data.getChannels(); ++channel)
            for (auto frame = static_cast<std::uint32_t>(resultFrames); frame < frames; ++frame)
                samples[channel * frames + frame] = 0.0F;
    }

    VorbisClip::VorbisClip(Audio& initAudio, const std::vector<std::byte>& initData):
        Sound{
            initAudio,
            initAudio.initData(std::unique_ptr<mixer::Data>(data = new VorbisData(initData))),
            Sound::Format::vorbis
        }
    {
    }
}
