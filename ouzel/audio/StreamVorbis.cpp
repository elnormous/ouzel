// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "StreamVorbis.hpp"
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

namespace ouzel
{
    namespace audio
    {
        StreamVorbis::StreamVorbis(const std::vector<uint8_t>& data)
        {
            vorbisStream = stb_vorbis_open_memory(data.data(), static_cast<int>(data.size()), nullptr, nullptr);
        }

        StreamVorbis::~StreamVorbis()
        {
            if (vorbisStream)
                stb_vorbis_close(vorbisStream);
        }

        void StreamVorbis::reset()
        {
            Stream::reset();

            stb_vorbis_seek_start(vorbisStream);
        }
    } // namespace audio
} // namespace ouzel
