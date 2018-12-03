// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "VorbisSound.hpp"
#include "VorbisStream.hpp"
#include "mixer/Source.hpp"
#include "mixer/SourceData.hpp"
#include "utils/Utils.hpp"

#include "stb_vorbis.c"

namespace ouzel
{
    namespace audio
    {
        class VorbisSource: public Source
        {
        public:
        };

        class VorbisSourceData: public SourceData
        {
        public:
        };

        VorbisSound::VorbisSound(Audio& initAudio, const std::vector<uint8_t>& initData):
            Sound(initAudio),
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

        std::shared_ptr<Stream> VorbisSound::createStream()
        {
            return std::make_shared<VorbisStream>(data);
        }

        void VorbisSound::readData(Stream* stream, uint32_t frames, std::vector<float>& result)
        {
            VorbisStream* streamVorbis = static_cast<VorbisStream*>(stream);

            uint32_t neededSize = frames * channels;
            uint32_t totalSize = 0;
            stb_vorbis* vorbisStream = streamVorbis->getVorbisStream();

            result.resize(neededSize);

            while (neededSize > 0)
            {
                if (vorbisStream->eof)
                    stream->reset();

                int resultFrames = stb_vorbis_get_samples_float_interleaved(vorbisStream, channels, result.data(), static_cast<int>(neededSize));
                totalSize += static_cast<uint32_t>(resultFrames) * channels;
                neededSize -= static_cast<uint32_t>(resultFrames) * channels;

                if (!stream->isRepeating()) break;
            }

            if (vorbisStream->eof)
                stream->reset();

            std::fill(result.begin() + totalSize, result.end(), 0.0F);
        }
    } // namespace audio
} // namespace ouzel
