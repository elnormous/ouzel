// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "VorbisSound.hpp"
#include "Audio.hpp"
#include "mixer/Stream.hpp"
#include "mixer/Source.hpp"
#include "utils/Utils.hpp"

#include "stb_vorbis.c"

namespace ouzel
{
    namespace audio
    {
        class VorbisData;

        class VorbisSource: public mixer::Stream
        {
        public:
            VorbisSource(VorbisData& vorbisData);

            ~VorbisSource()
            {
                if (vorbisStream)
                    stb_vorbis_close(vorbisStream);
            }

            void reset() override
            {
                stb_vorbis_seek_start(vorbisStream);
            }

            void getData(uint32_t frames, std::vector<float>& samples) override;

        private:
            stb_vorbis* vorbisStream = nullptr;
        };

        class VorbisData: public mixer::Source
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
                return std::unique_ptr<mixer::Stream>(new VorbisSource(*this));
            }

        private:
            std::vector<uint8_t> data;
        };

        VorbisSource::VorbisSource(VorbisData& vorbisData):
            Stream(vorbisData)
        {
            const std::vector<uint8_t>& data = vorbisData.getData();
            vorbisStream = stb_vorbis_open_memory(data.data(), static_cast<int>(data.size()), nullptr, nullptr);
        }

        void VorbisSource::getData(uint32_t frames, std::vector<float>& samples)
        {
            uint32_t neededSize = frames * source.getChannels();
            samples.resize(neededSize);

            uint32_t totalSize = 0;

            while (neededSize > 0)
            {
                if (vorbisStream->eof)
                    reset();

                int resultFrames = stb_vorbis_get_samples_float_interleaved(vorbisStream, source.getChannels(),
                                                                            samples.data() + totalSize, static_cast<int>(neededSize));
                totalSize += static_cast<uint32_t>(resultFrames) * source.getChannels();
                neededSize -= static_cast<uint32_t>(resultFrames) * source.getChannels();

                if (!isRepeating()) break;
            }

            if (vorbisStream->eof)
            {
                if (!isRepeating()) playing = false; // TODO: fire event
                reset();
            }

            std::fill(samples.begin() + totalSize, samples.end(), 0.0F);
        }

        VorbisSound::VorbisSound(Audio& initAudio, const std::vector<uint8_t>& initData):
            Sound(initAudio, initAudio.initSource([initData](){
                return std::unique_ptr<mixer::Source>(new VorbisData(initData));
            }))
        {
        }
    } // namespace audio
} // namespace ouzel
