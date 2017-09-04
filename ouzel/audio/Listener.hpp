// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/SoundOutput.hpp"

namespace ouzel
{
    namespace audio
    {
        class ListenerResource;

        class Listener: public SoundOutput
        {
        public:
            Listener();
            virtual ~Listener();

        private:
            ListenerResource* resource = nullptr;
        };
    } // namespace audio
} // namespace ouzel
