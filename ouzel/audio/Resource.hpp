// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <vector>
#include "utils/Noncopyable.hpp"

namespace ouzel
{
    namespace audio
    {
        class Resource: public Noncopyable
        {
        public:
            Resource() {}
            virtual ~Resource() {}

            virtual bool getData(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& result) = 0;
        };
    } // namespace audio
} // namespace ouzel
