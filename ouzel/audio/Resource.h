// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    namespace audio
    {
        class Audio;

        class Resource
        {
            friend Audio;
        public:
            Resource() {}
            virtual ~Resource() {}

        protected:
            virtual bool update() = 0;
        };
    } // audio
} // ouzel
