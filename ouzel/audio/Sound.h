// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    namespace audio
    {
        class Audio;

        class Sound
        {
            friend Audio;
        public:
            virtual ~Sound();

        protected:
            Sound();
        };
    } // namespace audio
} // namespace ouzel
