// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    namespace graphics
    {
        class Resource
        {
        public:
            virtual void free() {}
            virtual bool update() { return true; }
        };
    } // graphics
} // ouzel
