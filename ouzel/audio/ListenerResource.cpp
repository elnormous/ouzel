// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ListenerResource.hpp"

namespace ouzel
{
    namespace audio
    {
        bool ListenerResource::getData(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& result)
        {
            return true;
        }
    } // namespace audio
} // namespace ouzel
