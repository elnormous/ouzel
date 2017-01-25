// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <atomic>

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class Resource
        {
            friend Renderer;
        public:
            Resource() {}
            virtual ~Resource() {}

        protected:
            virtual bool upload() = 0;
        };
    } // graphics
} // ouzel
