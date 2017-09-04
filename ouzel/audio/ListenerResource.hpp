// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/OutputResource.hpp"

namespace ouzel
{
    namespace audio
    {
        class ListenerResource: public OutputResource
        {
        public:
            virtual bool getData(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& result) override;
        };
    } // namespace audio
} // namespace ouzel
