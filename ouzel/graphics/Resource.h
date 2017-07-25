// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

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
        };
    } // graphics
} // ouzel
