// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>

namespace ouzel
{
    namespace graphics
    {
        class Resource: public std::enable_shared_from_this<Resource>
        {
        public:
            virtual void free() {}
            virtual bool update() { return true; }
            virtual bool upload() { return true; }
        };
    } // graphics
} // ouzel
