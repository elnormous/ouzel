// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_VORBISSTREAM_HPP
#define OUZEL_AUDIO_VORBISSTREAM_HPP

#include <cstdint>
#include <vector>
#include "Stream.hpp"

struct stb_vorbis;

namespace ouzel
{
    namespace audio
    {
        class VorbisStream final: public Stream
        {
        public:
            explicit VorbisStream(const std::vector<uint8_t>& data);
            ~VorbisStream();
            void reset() override;

            inline stb_vorbis* getVorbisStream() const { return vorbisStream; }

        private:
            stb_vorbis* vorbisStream = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_VORBISSTREAM_HPP
