// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundDataVorbis.hpp"
#include "StreamVorbis.hpp"
#include "core/Engine.hpp"
#include "files/FileSystem.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"
#include "stb_vorbis.c"

namespace ouzel
{
    namespace audio
    {
        SoundDataVorbis::SoundDataVorbis()
        {
        }

        bool SoundDataVorbis::init(const std::string& newFilename)
        {
            filename = newFilename;

            std::vector<uint8_t> newData;
            if (!sharedEngine->getFileSystem()->readFile(newFilename, newData))
            {
                return false;
            }

            return init(newData);
        }

        bool SoundDataVorbis::init(const std::vector<uint8_t>& newData)
        {
            data = newData;

            stb_vorbis* vorbisStream = stb_vorbis_open_memory(data.data(), static_cast<int>(data.size()), nullptr, nullptr);
            stb_vorbis_info info = stb_vorbis_get_info(vorbisStream);

            channels = static_cast<uint16_t>(info.channels);
            sampleRate = info.sample_rate;

            stb_vorbis_close(vorbisStream);

            return true;
        }

        std::shared_ptr<Stream> SoundDataVorbis::createStream()
        {
            return std::make_shared<StreamVorbis>(data);
        }

        bool SoundDataVorbis::getData(Stream* stream, uint32_t frames, std::vector<float>& result)
        {
            StreamVorbis* streamVorbis = static_cast<StreamVorbis*>(stream);

            uint32_t neededSize = frames * channels;
            stb_vorbis* vorbisStream = streamVorbis->getVorbisStream();

            if (vorbisStream->eof)
            {
                stb_vorbis_seek_start(vorbisStream);
            }

            result.resize(neededSize);

            while (neededSize > 0)
            {
                if (vorbisStream->eof)
                {
                    stb_vorbis_seek_start(vorbisStream);
                }

                int resultFrames = stb_vorbis_get_samples_float_interleaved(vorbisStream, channels, result.data(), neededSize);

                if (stream->isRepeating())
                {
                    neededSize -= resultFrames * channels;
                }
                else
                {
                    result.resize(resultFrames * channels);
                    break;
                }
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
