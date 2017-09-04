// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/Output.hpp"

namespace ouzel
{
    namespace audio
    {
        class ListenerResource;

        class Listener: public Output
        {
        public:
            Listener();
            virtual ~Listener();

        private:
            ListenerResource* resource = nullptr;
        };
    } // namespace audio
} // namespace ouzel
