// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "StreamVorbis.h"
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
            {
                stb_vorbis_close(vorbisStream);
            }
        }

        void StreamVorbis::reset()
        {
            stb_vorbis_seek(vorbisStream, 0);
        }
    } // namespace audio
} // namespace ouzel
