// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    namespace audio
    {
        class Audio
        {
        public:
            virtual ~Audio() {}

            virtual bool init();
        };
    } // namespace audio
} // namespace ouzel
