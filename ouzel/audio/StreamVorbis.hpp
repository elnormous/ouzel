// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <vector>
#include "Stream.hpp"

struct stb_vorbis;

namespace ouzel
{
    namespace audio
    {
        class StreamVorbis: public Stream
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
