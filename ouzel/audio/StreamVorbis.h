// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include "Stream.h"

namespace ouzel
{
    namespace audio
    {
        class StreamVorbis: public Stream
        {
        public:
            virtual void reset() override;

            uint32_t getOffset() const { return offset; }
            void setOffset(uint32_t newOffset) { offset = newOffset; }

        private:
            uint32_t offset = 0;
        };
    } // namespace audio
} // namespace ouzel
