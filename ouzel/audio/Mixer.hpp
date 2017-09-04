// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/Output.hpp"

namespace ouzel
{
    namespace audio
    {
        class MixerResource;

        class Mixer: public Output
        {
        public:
            Mixer();
            virtual ~Mixer();

        private:
            MixerResource* resource = nullptr;
        };
    } // namespace audio
} // namespace ouzel
