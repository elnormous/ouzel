// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "VorbisClip.hpp"
#include "Audio.hpp"
#include "mixer/Data.hpp"
#include "mixer/Stream.hpp"
#include "utils/Utils.hpp"

#include "stb_vorbis.c"

namespace ouzel
{
    namespace audio
    {
        class VorbisData;

        class VorbisStream final: public mixer::Stream
        {
        public:
            VorbisStream(VorbisData& vorbisData);

            ~VorbisStream()
            {
                if (vorbisStream)
                    stb_vorbis_close(vorbisStream);
            }

            void reset() override
            {
                stb_vorbis_seek_start(vorbisStream);
            }

            void getSamples(uint32_t frames, std::vector<float>& samples) override;

        private:
            stb_vorbis* vorbisStream = nullptr;
        };

        class VorbisData final: public mixer::Data
        {
        public:
            VorbisData(const std::vector<uint8_t>& initData):
                data(initData)
            {
                stb_vorbis* vorbisStream = stb_vorbis_open_memory(data.data(), static_cast<int>(data.size()), nullptr, nullptr);

                if (!vorbisStream)
                    throw std::runtime_error("Failed to load Vorbis stream");

                stb_vorbis_info info = stb_vorbis_get_info(vorbisStream);

                channels = static_cast<uint16_t>(info.channels);
                sampleRate = info.sample_rate;

                stb_vorbis_close(vorbisStream);
            }

            const std::vector<uint8_t>& getData() const { return data; }

            std::unique_ptr<mixer::Stream> createStream() override
            {
                return std::unique_ptr<mixer::Stream>(new VorbisStream(*this));
            }

        private:
            std::vector<uint8_t> data;
        };

        VorbisStream::VorbisStream(VorbisData& vorbisData):
            Stream(vorbisData)
        {
            vorbisStream = stb_vorbis_open_memory(vorbisData.getData().data(),
                                                  static_cast<int>(vorbisData.getData().size()),
                                                  nullptr, nullptr);
        }

        void VorbisStream::getSamples(uint32_t frames, std::vector<float>& samples)
        {
            uint32_t neededSize = frames * data.getChannels();
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
                        throw std::runtime_error("Unsupported channel count");
                }

                resultFrames = stb_vorbis_get_samples_float(vorbisStream, data.getChannels(),
                                                            channelData.data(), static_cast<int>(frames));
            }

            if (vorbisStream->eof)
            {
                playing = false; // TODO: fire event
                reset();
            }

            for (uint32_t channel = 0; channel < data.getChannels(); ++channel)
                for (uint32_t frame = static_cast<uint32_t>(resultFrames); frame < frames; ++frame)
                    samples[channel * frames + frame] = 0.0F;
        }

        VorbisClip::VorbisClip(Audio& initAudio, const std::vector<uint8_t>& initData):
            Sound(initAudio,
                  initAudio.initData(std::unique_ptr<mixer::Data>(data = new VorbisData(initData))),
                  Sound::Format::VORBIS)
        {
        }
    } // namespace audio
} // namespace ouzel
