// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <vector>
#include "Stream.hpp"

struct stb_vorbis;

namespace ouzel
{
    namespace audio
    {
        class StreamVorbis final: public Stream
        {
        public:
            explicit StreamVorbis(const std::vector<uint8_t>& data);
            virtual ~StreamVorbis();
            virtual void reset() override;

            inline stb_vorbis* getVorbisStream() const { return vorbisStream; }

        private:
            stb_vorbis* vorbisStream = nullptr;
        };
    } // namespace audio
} // namespace ouzel
