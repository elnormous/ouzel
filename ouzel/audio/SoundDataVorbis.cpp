// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundDataVorbis.h"
#include "StreamVorbis.h"
#include "core/Engine.h"
#include "files/FileSystem.h"
#include "utils/Log.h"
#include "utils/Utils.h"
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

        std::unique_ptr<Stream> SoundDataVorbis::createStream()
        {
            std::unique_ptr<Stream> stream(new StreamVorbis(data));

            return stream;
        }

        bool SoundDataVorbis::getData(Stream* stream, uint32_t samples, std::vector<float>& result)
        {
            StreamVorbis* streamVorbis = static_cast<StreamVorbis*>(stream);

            result.resize(samples);

            int n = stb_vorbis_get_samples_float_interleaved(streamVorbis->getVorbisStream(), channels, result.data(), samples);

            result.resize(n * channels);

            return true;
        }
    } // namespace audio
} // namespace ouzel
